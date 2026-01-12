#include <stdlib.h>
#include "pad.h"
#include "esp_log.h"
#include <freertos/queue.h>
#include "esp_timer.h"
#include <math.h>
#include "storage/nvs_manager.h"
#include "model/pad_persist_t.h"
#include <string.h>

pad_t *create_pad(adc_type_t type,
                  int id,
                  const char *name,
                  int pin,
                  int note,
                  int maxValue,
                  QueueHandle_t queue)
{
    pad_t *pad = (pad_t *)malloc(sizeof(pad_t));
    if (!pad)
        return NULL;

    // id
    pad->id = id;
    strncpy(pad->name, name, sizeof(pad->name));
    pad->name[sizeof(pad->name) - 1] = '\0';

    // ADC
    pad->piezo = create_adc(type, pin);
    pad->adc_res = 4095;

    // Configs default
    pad->maxValue = maxValue,
    pad->note = note;
    pad->eventQueue = queue;

    // --- FILTER ---- //
    pad->velocity = 0;
    pad->time_scan_peak = 0;
    pad->time_end = 0;
    pad->loopTimes = 0;
    pad->state = 0;
    pad->adc_peak = 0;
    pad->sensitivity = 0;
    pad->velocity_curve = 1.0f;
    pad->threshold = 100;

    // --- PEAK ---- //
    pad->peak_hold_time = (600);

    // --- retrigger ---- //
    // pad->duration_retrigger_scan_time = (10 * 1000);
    pad->retrigger_min_us = 6000;
    pad->retrigger_max_us = 20000;
    pad->retrigger_curve = 1.0f;
    pad->retrigger_current_us = pad->retrigger_min_us;
    // pad->retrigger_aggressiveness = 0;
    return pad;
}

void update_pad_wrapper(void *obj)
{
    update_pad((pad_t *)obj);
}

void update_pad(pad_t *pad)
{

    if (!pad || !pad->piezo)
        return;

    read_adc(pad->piezo);
    int pzValue = pad->piezo->value;
    // printf("update %d\n", pzValue);
    // vTaskDelay(pdMS_TO_TICKS(10));

    // threshold
    if (pad->state == 0 && pzValue < pad->threshold)
    {
        return;
    }

    uint64_t time_now = esp_timer_get_time();

    //---detec peak---//

    // start
    if (pad->state == 0)
    {
        if (time_now - pad->time_end < pad->retrigger_current_us)
        {
            // printf("retrigger \n");
            return;
        }

        pad->time_scan_peak = time_now;
        pad->adc_peak = pzValue;
        pad->state = 1;
        pad->time_last_change = time_now;
        // pad->loopTimes = 1;
        return;
    }

    if (pad->state == 1)
    {
        if (pzValue > pad->adc_peak)
        {
            pad->adc_peak = pzValue;
            pad->time_last_change = time_now;
        }

        // peak
        else
        {
            if (time_now - pad->time_last_change > pad->peak_hold_time)
            {
                // // printf("[PAD] adc peak: %d\n",pad->adc_peak);
                // float norm = (float)pad->adc_peak / (float)pad->adc_res;

                // float curve = 0.80f;
                // float boosted = powf(norm, curve);

                // float sens = (float)pad->sensitivity / 100.0f;
                // float adjusted = boosted + (1.0f - boosted) * sens;
                // uint8_t velocity = (uint8_t)(fminf(adjusted * 127, 127));

                // // calc retrigger

                // pad->retrigger_current_us =
                //     pad->retrigger_min_us +
                //     (uint32_t)((pad->retrigger_max_us - pad->retrigger_min_us) * impact);

                // ---------- VELOCITY (musical) ----------
                // printf("[ENGINEPAD] adc: %d \n" , pad->adc_peak);
                float vel_norm = (float)pad->adc_peak / (float)pad->adc_res;
                if (vel_norm > 1.0f)
                    vel_norm = 1.0f;

                
                float boosted = powf(vel_norm, pad->velocity_curve);

                float sens = (float)pad->sensitivity / 100.0f;
                float adjusted = boosted + (1.0f - boosted) * sens;

                uint8_t velocity = (uint8_t)(fminf(adjusted * 127.0f, 127.0f));

                // ---------- RETRIGGER (físico) ----------
                float rt_norm = (float)(pad->adc_peak - pad->threshold) /
                                (float)(pad->adc_res - pad->threshold);

                if (rt_norm < 0.0f)
                    rt_norm = 0.0f;
                if (rt_norm > 1.0f)
                    rt_norm = 1.0f;

                // curva simples e controlável
                float rt_impact = powf(rt_norm, pad->retrigger_curve);

                pad->retrigger_current_us =
                    pad->retrigger_min_us +
                    (uint32_t)((pad->retrigger_max_us - pad->retrigger_min_us) * rt_impact);

                midi_event_t event;
                event.note = pad->note;
                event.velocity = velocity;
                event.noteOff = false;
                xQueueSend(pad->eventQueue, &event, portMAX_DELAY);

                pad->state = 2;
                pad->time_end = time_now;
            }
        }
    }

    if (pad->state == 2)
    {

        if (pzValue < pad->threshold)
        {
            pad->state = 0;
        }
    }

    // scan
    //     if (pad->loopTimes > 0)
    //     {
    //         //   printf("scan \n");
    //         if (pzValue > pad->adc_peak)
    //         {

    //             pad->adc_peak = pzValue;
    //         }

    //         pad->loopTimes++;

    //         if (time_now - pad->time_scan_peak > pad->duration_scan_time)
    //         {
    //             uint8_t midi_velocity = (pad->adc_peak * 127) / pad->adc_res;

    //             midi_event_t event;
    //             event.note = pad->note;
    //             event.velocity = midi_velocity;
    //             event.noteOff = false;
    //             xQueueSend(pad->eventQueue, &event, portMAX_DELAY);

    //             pad->time_end = time_now;
    //             pad->loopTimes = 0;
    //         }
    //     }
}

void delete_pad(pad_t *pad)
{
    if (!pad)
        return;

    // NVS
    char key[8];
    snprintf(key, sizeof(key), "pad%d", pad->id);
    nvs_manager_erase_key("pads", key);

    // delete adc
    if (pad->piezo)
        delete_adc(pad->piezo);

    free(pad);
}

void pad_save(pad_t *pad)
{
    if (!pad)
        return;

    pad_persist_t data = {
        .id = pad->id,
        .threshold = pad->threshold,
        .note = pad->note,
        .sensitivity = pad->sensitivity,
        .velocity_curve = pad->velocity_curve,
        .peak_hold_time = pad->peak_hold_time,
        .retrigger_min_us = pad->retrigger_min_us,
        .retrigger_max_us = pad->retrigger_max_us,
        .retrigger_curve = pad->retrigger_curve
    };

    strncpy(data.name, pad->name, sizeof(data.name));
    data.name[sizeof(data.name) - 1] = '\0';

    char key[8];
    snprintf(key, sizeof(key), "pad%d", pad->id);

    nvs_write_blob("pads", key, &data, sizeof(data));
}

void pad_load(pad_t *pad)
{
    if (!pad)
        return;

    pad_persist_t data;
    char key[8];
    snprintf(key, sizeof(key), "pad%d", pad->id);

    if (!nvs_read_blob("pads", key, &data, sizeof(data)))
        return;

    pad->threshold = data.threshold;
    pad->note = data.note;
    pad->sensitivity = data.sensitivity;
    pad->velocity_curve = data.velocity_curve;
    pad->peak_hold_time = data.peak_hold_time;

    pad->retrigger_min_us = data.retrigger_min_us;
    pad->retrigger_max_us = data.retrigger_max_us;
    pad->retrigger_curve = data.retrigger_curve;

    strncpy(pad->name, data.name, sizeof(pad->name));
    pad->name[sizeof(pad->name) - 1] = '\0';
}