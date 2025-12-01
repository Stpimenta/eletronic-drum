#pragma once
#include "hardware/adc_reader/adc_reader.h"
#include "model/struct_midi/midi_event_t.h"
#include "freertos/FreeRTOS.h"



typedef struct 
{
    adc_reader_t * piezo;

    int threshold;
    int note;
    int maxValue;

    bool wasHit;
    QueueHandle_t eventQueue;

}pad_t;

void read_pad(pad_t *pad);
pad_t* create_pad(adc_type_t type, int pin, int threshold, int note, int maxValue, QueueHandle_t queue);
void delete_pad(pad_t *pad);
void update_pad(pad_t *pad);