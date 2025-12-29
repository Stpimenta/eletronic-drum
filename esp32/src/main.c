#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "hardware/adc_reader/adc_reader.h"
#include "queue/midi_queue.h"
#include "process/process_timer.h"
#include "engine/pad/pad.h"
#include "process/process_uart.h"
#include "controller_uart/pad_controller.h"
#include"controller_uart/engine_controller.h"
#include "storage/nvs_manager.h"

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



#define NUM_PADS 1
pad_t *pads[NUM_PADS];


void app_main(void)
{
    setup_adc();
    nvs_manager_init();

    // create queue for process midi events
    QueueHandle_t midiQueue = create_midi_queue();

    // parts of drum
    pad_t *pad1 = create_pad(ADC1_TYPE,1,"snare",ADC1_CHANNEL_6, 100, 38, 4095, midiQueue);
    pads[0] = pad1;

    //timer and read tasks 
    int numEngineParts = 1;
    engine_t *engineParts = malloc(sizeof(engine_t) * numEngineParts);
    engineParts[0].type = ENGINE_PAD;
    engineParts[0].object = pad1;
    engineParts[0].update = update_pad_wrapper;

    //controller uart - communication with esp01 web page
    pad_controller_init(pads, NUM_PADS);
    engine_controller_init(engineParts,numEngineParts);

    // start tasks
    start_process_timer(engineParts,numEngineParts);
    start_queue_midi_task(0);
    start_process_uart(UART_NUM_1,16,17);


}