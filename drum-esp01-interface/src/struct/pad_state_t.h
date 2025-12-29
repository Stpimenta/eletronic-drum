#pragma once

#define PAD_NAME_LEN 32

typedef struct
{
    int id;
    char name[PAD_NAME_LEN];

    int threshold;
    int note;
    int sensitivity;
    int peak_hold;
    int retrigger;
} pad_state_t;
