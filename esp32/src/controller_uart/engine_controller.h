#pragma once
#include <stdbool.h>
#include <stddef.h>
#include "engine/baseType/sensor_type.h"

void engine_controller_init(engine_t *engines, int count);
bool engine_controller_get_all(char *out, size_t max_len);

