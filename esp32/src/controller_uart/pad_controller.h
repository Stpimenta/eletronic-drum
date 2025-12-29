#pragma once
#include <stdbool.h>
#include "engine/pad/pad.h"

void pad_controller_init(pad_t **pads, int count);
bool pad_controller_handle_set(const char *cmd);
