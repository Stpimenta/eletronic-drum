#pragma once
#include "driver/adc.h"

typedef enum { ADC1_TYPE = 1, ADC2_TYPE = 2 } adc_type_t;

typedef struct
{
    adc_type_t type;
    int pin;
    int value;
} adc_reader_t;  

void read_adc(adc_reader_t *reader);
adc_reader_t* create_adc(adc_type_t type, int pin);
void delete_adc(adc_reader_t *reader);
