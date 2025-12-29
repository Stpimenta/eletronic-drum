#pragma once
#include <stdint.h>
typedef struct
{
    int id;
    char name[32];
    int threshold;         
    int note;               
    int sensitivity;       
    int peak_hold;          
    int retrigger;          
} pad_persist_t;
