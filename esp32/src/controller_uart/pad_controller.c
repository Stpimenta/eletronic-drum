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
    int value;

    int parsed = sscanf(cmd, "SET PAD %d %31s %d", &id, param, &value);
    if (parsed != 3)
        return false;

    param[strcspn(param, "\r\n ")] = 0;

    printf("[PAD CTRL] id=%d param=%s value=%d\n", id, param, value);
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

    if (strcmp(param, "THRESHOLD") == 0)
    {
        if (pad->threshold != value)
        {
            pad->threshold = value;
            changed = true;
        }
    }
    else if (strcmp(param, "NOTE") == 0)
    {
        if (pad->note != value)
        {
            pad->note = value;
            changed = true;
        }
    }
    else if (strcmp(param, "SENSITIVITY") == 0)
    {
        if (pad->sensitivity != value)
        {
            pad->sensitivity = value;
            changed = true;
        }
    }
    else if (strcmp(param, "PEAK_HOLD") == 0)
    {
        if (pad->peak_hold_time != value)
        {
            pad->peak_hold_time = value;
            changed = true;
        }
    }
    else if (strcmp(param, "RETRIGGER") == 0)
    {
        if (pad->duration_retrigger_scan_time != value)
        {
            pad->duration_retrigger_scan_time = value;
            changed = true;
        }
    }

    if (changed)
    {
        pad_save(pad);
    }

    return changed;
}
