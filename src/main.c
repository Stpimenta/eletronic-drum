#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "hardware/adc_reader/adc_reader.h"
#include "engine/pad/pad.h"




void setup_adc()
{
    // ADC1 - global res
    adc1_config_width(ADC_WIDTH_BIT_12);

    // ADC1_CHANNEL_0 = GPIO36
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
    // ADC1_CHANNEL_1 = GPIO37
    adc1_config_channel_atten(ADC1_CHANNEL_1, ADC_ATTEN_DB_11);
    // ADC1_CHANNEL_2 = GPIO38
    adc1_config_channel_atten(ADC1_CHANNEL_2, ADC_ATTEN_DB_11);
    // ADC1_CHANNEL_3 = GPIO39
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_11);
    // ADC1_CHANNEL_4 = GPIO32
    adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_11);
    // ADC1_CHANNEL_5 = GPIO33
    adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11);
    // ADC1_CHANNEL_6 = GPIO34
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);
    // ADC1_CHANNEL_7 = GPIO35
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);

    // ADC2 - only atten || adc2 res is config in read time
    // ADC2_CHANNEL_0 = GPIO4
    adc2_config_channel_atten(ADC2_CHANNEL_0, ADC_ATTEN_DB_11);
    // ADC2_CHANNEL_1 = GPIO0
    adc2_config_channel_atten(ADC2_CHANNEL_1, ADC_ATTEN_DB_11);
    // ADC2_CHANNEL_2 = GPIO2
    adc2_config_channel_atten(ADC2_CHANNEL_2, ADC_ATTEN_DB_11);
    // ADC2_CHANNEL_3 = GPIO15
    adc2_config_channel_atten(ADC2_CHANNEL_3, ADC_ATTEN_DB_11);
    // ADC2_CHANNEL_4 = GPIO13
    adc2_config_channel_atten(ADC2_CHANNEL_4, ADC_ATTEN_DB_11);
    // ADC2_CHANNEL_5 = GPIO12
    adc2_config_channel_atten(ADC2_CHANNEL_5, ADC_ATTEN_DB_11);
    // ADC2_CHANNEL_6 = GPIO14
    adc2_config_channel_atten(ADC2_CHANNEL_6, ADC_ATTEN_DB_11);
    // ADC2_CHANNEL_7 = GPIO27
    adc2_config_channel_atten(ADC2_CHANNEL_7, ADC_ATTEN_DB_11);
    // ADC2_CHANNEL_8 = GPIO25
    adc2_config_channel_atten(ADC2_CHANNEL_8, ADC_ATTEN_DB_11);
    // ADC2_CHANNEL_9 = GPIO26
    adc2_config_channel_atten(ADC2_CHANNEL_9, ADC_ATTEN_DB_11);
}



#define QUEUE_SIZE 15
QueueHandle_t midiQueue;
void midi_task(void *arg)
{
    midi_event_t event;
    while (1)
    {
        if (xQueueReceive(midiQueue, &event, portMAX_DELAY) == pdTRUE)
        {
            // Simula envio de MIDI
            if (!event.noteOff)
            {
                printf("MIDI Note On: %d, velocity: %d\n", event.note, event.velocity);
            }
            else
            {
                printf("MIDI Note Off: %d\n", event.note);
            }
        }
    }
}

void app_main(void)
{
    setup_adc();

    // Cria fila
    midiQueue = xQueueCreate(QUEUE_SIZE, sizeof(midi_event_t));

    // Cria pad
   pad_t *pad1 = create_pad(ADC1_TYPE, ADC1_CHANNEL_6, 100, 36, 4095, midiQueue);

    pad1->eventQueue = midiQueue;

    // Cria task MIDI
    xTaskCreate(midi_task, "midi_task", 2048, NULL, 5, NULL);

    while (1)
    {
        // Atualiza pad e envia eventos
        update_pad(pad1);
        vTaskDelay(pdMS_TO_TICKS(10)); // 10ms loop
    }
}