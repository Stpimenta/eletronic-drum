#include "engine_controller.h"
#include "engine/pad/pad.h"
#include <stdio.h>
#include <string.h>

static engine_t *s_engines = NULL;
static int s_engine_count = 0;

void engine_controller_init(engine_t *engines, int count)
{
    if (!engines || count <= 0)
        return;

    s_engines = engines;
    s_engine_count = count;
}

bool engine_controller_get_all(char *out, size_t max_len)
{
    printf("get all \n");
    if (!out || max_len == 0 || !s_engines)
        return false;

    size_t used = 0;
    int n;

    n = snprintf(out + used, max_len - used, "BEGIN\n");
    if (n < 0 || n >= max_len)
        return false;
    used += n;

    for (int i = 0; i < s_engine_count; i++)
    {
        engine_t *eng = &s_engines[i];

        if (eng->type == ENGINE_PAD)
        {
            pad_t *pad = (pad_t *)eng->object;

            n = snprintf(
                out + used,
                max_len - used,
                "PAD %d %s %d %d %d %d %d\n",
                pad->id,
                pad->name,
                pad->threshold,
                pad->note,
                pad->sensitivity,
                pad->peak_hold_time,
                pad->duration_retrigger_scan_time);

            if (n < 0 || used + n >= max_len)
                return false;

            used += n;
        }
    }

    n = snprintf(out + used, max_len - used, "END\n");


    if (n < 0 || used + n >= max_len)
        return false;


    printf("ENGINE RESPONSE:\n%s", out);

    return true;
}
