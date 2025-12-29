#include "esp_timer.h"
#include <stdlib.h>
#include "process_timer.h"

static engine_t *g_engineParts = NULL;
static size_t g_count = 0;
static esp_timer_handle_t g_timer = NULL;

static void engine_timer_callback(void *arg)
{
    for (size_t i = 0; i < g_count; i++)
    {
        engine_t *engine = &g_engineParts[i];
        engine->update(engine->object);
    }
}

void start_process_timer(engine_t *engineParts, size_t count)
{
    g_engineParts = engineParts;
    g_count = count;

    const esp_timer_create_args_t timer_args = {
        .callback = engine_timer_callback,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "engine_timer",
    };

    esp_timer_create(&timer_args, &g_timer);


    esp_timer_start_periodic(g_timer, 200);
}

void stop_process_timer()
{
    if (g_timer)
    {
        esp_timer_stop(g_timer);
        esp_timer_delete(g_timer);
        g_timer = NULL;
    }
}
