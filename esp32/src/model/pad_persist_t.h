#pragma once
#include <stdint.h>

typedef struct
{
    int32_t id;

    char name[32];

    int32_t threshold;
    int32_t note;

    int32_t sensitivity;
    float   velocity_curve;

    int32_t peak_hold_time;

    int32_t retrigger_min_us;
    int32_t retrigger_max_us;
    float   retrigger_curve;

} pad_persist_t;
