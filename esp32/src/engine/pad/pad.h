#pragma once
#include "hardware/adc_reader/adc_reader.h"
#include "model/struct_midi/midi_event_t.h"
#include "freertos/FreeRTOS.h"

typedef struct
{
    // =====================================================
    // ID
    // =====================================================
    int id;                 
    char name[32];
    // =====================================================
    // HARDWARE
    // =====================================================
    adc_reader_t *piezo;    // PIEZO            
    QueueHandle_t eventQueue; // QUEUE

    // =====================================================
    // CONFIG
    // =====================================================
    int note;              
    int threshold;          
    int maxValue;           
    int sensitivity;        
    int peak_hold_time;     
    int duration_retrigger_scan_time; 

    // =====================================================
    // RUNTIME
    // =====================================================
    int adc_peak;           
    uint8_t velocity;       
    uint64_t time_scan_peak;
    uint64_t time_end;
    uint64_t time_last_change;
    int loopTimes;
    int state;
    int adc_res;                                

} pad_t;

pad_t *create_pad(adc_type_t type,
                  int id,
                  const char *name, 
                  int pin,
                  int threshold,
                  int note,
                  int maxValue,
                  QueueHandle_t queue);
void delete_pad(pad_t *pad);
void update_pad(pad_t *pad);

void pad_save(pad_t *pad);
void pad_load(pad_t *pad);