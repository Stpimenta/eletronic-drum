#include "adc_reader.h"
#include <stdlib.h>

void read_adc(adc_reader_t *reader)
{
    if (!reader) return;

    if (reader->type == ADC1_TYPE)
    {
        reader->value = adc1_get_raw(reader->pin);
    }
    else
    {
        int raw;
        esp_err_t r = adc2_get_raw((adc2_channel_t)reader->pin, ADC_WIDTH_BIT_12, &raw);
        if (r == ESP_OK)
        {
            reader->value = raw;
        }
        else
        {
            reader->value = -1; 
        }
    }
}

adc_reader_t* create_adc(adc_type_t type, int pin)
{
    adc_reader_t *reader = (adc_reader_t*)malloc(sizeof(adc_reader_t));
    if (!reader) return NULL;

    reader->type = type;
    reader->pin = pin;
    reader->value = 0;
    return reader;
}

void delete_adc(adc_reader_t *reader)
{
    if (!reader) return;
    free(reader);
    reader = NULL;
}
