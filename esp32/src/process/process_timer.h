#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "engine/pad/pad.h"
#include <stddef.h>
#include "engine/baseType/sensor_type.h"



typedef struct {
    engine_t *engineParts;  
    size_t count;      
} process_task_args_t;

void start_process_timer(engine_t *engineParts, size_t count);
void stop_process_timer();
