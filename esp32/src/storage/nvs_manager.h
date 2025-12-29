#pragma once
#include <stddef.h>
#include <stdbool.h>

void nvs_manager_init(void);

bool nvs_write_blob(const char *namespace,const char *key, const void *data, size_t size);

bool nvs_read_blob(const char *namespace, const char *key, void *data, size_t size);

bool nvs_manager_erase_key(const char *ns, const char *key);

