#include "pad_controller.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static pad_t **s_pads = NULL;
static int s_pad_count = 0;

void pad_controller_init(pad_t **pads, int count)
{
    if (!pads || count <= 0)
        return;

    s_pads = pads;
    s_pad_count = count;

    for (int i = 0; i < s_pad_count; i++)
    {
        if (s_pads[i])
            pad_load(s_pads[i]);
    }
}

bool pad_controller_handle_set(const char *cmd)
{
    if (!cmd)
        return false;

    int id;
    char param[32];
    char value_str[32];

    int parsed = sscanf(cmd, "SET PAD %d %31s %31s", &id, param, value_str);
    if (parsed != 3)
        return false;

    param[strcspn(param, "\r\n ")] = 0;
    value_str[strcspn(value_str, "\r\n ")] = 0;

    // printf("[PAD CTRL] RECEIVED -> PAD ID: %d, PARAM: %s, VALUE: %s\n", id, param, value_str);

    pad_t *pad = NULL;
    for (int i = 0; i < s_pad_count; i++)
    {
        if (s_pads[i]->id == id)
        {
            pad = s_pads[i];
            break;
        }
    }

    if (!pad)
    {
        printf("[PAD CTRL] pad NOT FOUND id=%d\n", id);
        return false;
    }

    bool changed = false;

    /* ---------- INT PARAMS ---------- */

    int ivalue = atoi(value_str);

    if (strcmp(param, "THRESHOLD") == 0)
    {
        if (pad->threshold != ivalue)
        {
            pad->threshold = ivalue;
            changed = true;
        }
    }
    else if (strcmp(param, "NOTE") == 0)
    {
        if (pad->note != ivalue)
        {
            pad->note = ivalue;
            changed = true;
        }
    }
    else if (strcmp(param, "SENSITIVITY") == 0)
    {
        if (pad->sensitivity != ivalue)
        {
            pad->sensitivity = ivalue;
            changed = true;
        }
    }
    else if (strcmp(param, "PEAK_HOLD") == 0)
    {
        if (pad->peak_hold_time != ivalue)
        {
            pad->peak_hold_time = ivalue;
            changed = true;
        }
    }
    else if (strcmp(param, "RETRIGGER_MIN") == 0)
    {
        if (pad->retrigger_min_us != ivalue)
        {
            pad->retrigger_min_us = ivalue;
            changed = true;
        }
    }
    else if (strcmp(param, "RETRIGGER_MAX") == 0)
    {
        if (pad->retrigger_max_us != ivalue)
        {
            pad->retrigger_max_us = ivalue;
            changed = true;
        }
    }

    /* ---------- FLOAT PARAMS ---------- */

    else if (strcmp(param, "VELOCITY_CURVE") == 0)
    {
        float fvalue = strtof(value_str, NULL);
        if (pad->velocity_curve != fvalue)
        {
            pad->velocity_curve = fvalue;
            changed = true;
        }
    }
    else if (strcmp(param, "RETRIGGER_CURVE") == 0)
    {
        float fvalue = strtof(value_str, NULL);
        if (pad->retrigger_curve != fvalue)
        {
            pad->retrigger_curve = fvalue;
            changed = true;
        }
    }
    else
    {
        printf("[PAD CTRL] UNKNOWN PARAM %s\n", param);
        return false;
    }

    if (changed)
    {
        pad_save(pad);
    }

    return changed;
}