#include <stdlib.h>
#include "pad.h"
#include "esp_log.h"
#include <freertos/queue.h>

pad_t *create_pad(adc_type_t type, int pin, int threshold, int note, int maxValue, QueueHandle_t queue)
{

    pad_t *pad = (pad_t *)malloc(sizeof(pad_t));
    if (!pad)
        return NULL;

    // create adc
    pad->piezo = create_adc(type, pin);
    pad->threshold = threshold;
    pad->maxValue = maxValue;
    pad->note = note;
    pad->wasHit = false;
    pad->eventQueue = queue;

    return pad;
}

void update_pad(pad_t *pad)
{

    if (!pad || !pad->piezo)
        return;

    read_adc(pad->piezo);
    int pzValue = pad->piezo->value;

    // detect hit
    if (!pad->wasHit && pzValue > pad->threshold)
    {

        int velocity = (pzValue * 127) / pad->maxValue;
        if (velocity > 127) velocity = 127;
        
        midi_event_t eventMidi = {
            .note = pad->note,
            .velocity = velocity,
            .noteOff = false
        };

        if (pad->eventQueue)
        {
            xQueueSend(pad->eventQueue, &eventMidi, portMAX_DELAY); // mudar depois para testar pmds
        }

        pad->wasHit = true;
    }

    if (pad->wasHit && pzValue < pad->threshold) // isso deve ser revisado quando tiver muitos pads pois pode ter problemas detectando outros pads na abteria montada
    {
        pad->wasHit = false;
    }
}

void delete_pad(pad_t *pad)
{
    if (!pad)
        return;

    // delete adc
    if (pad->piezo)
        delete_adc(pad->piezo);

    free(pad);
}
