// midi_queue.h
#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include "engine/pad/pad.h"

#define QUEUE_SIZE 15

QueueHandle_t create_midi_queue();
QueueHandle_t get_midi_queue();
void delete_midi_queue();
void start_queue_midi_task(int cpuCore);
void update_pad_wrapper(void *obj);
