#include "nvs_manager.h"
#include "nvs_flash.h"
#include "nvs.h"

void nvs_manager_init(void)
{
    esp_err_t err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
        err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        nvs_flash_erase();
        nvs_flash_init();
    }
}

bool nvs_write_blob(const char *ns, const char *key, const void *data, size_t size)
{
    nvs_handle_t handle;

    if (nvs_open(ns, NVS_READWRITE, &handle) != ESP_OK)
        return false;

    if (nvs_set_blob(handle, key, data, size) != ESP_OK)
    {
        nvs_close(handle);
        return false;
    }

    nvs_commit(handle);
    nvs_close(handle);
    return true;
}

bool nvs_read_blob(const char *ns, const char *key, void *data, size_t size)
{
    nvs_handle_t handle;
    size_t required = size;

    if (nvs_open(ns, NVS_READONLY, &handle) != ESP_OK)
        return false;

    if (nvs_get_blob(handle, key, data, &required) != ESP_OK)
    {
        nvs_close(handle);
        return false;
    }

    nvs_close(handle);
    return true;
}

bool nvs_manager_erase_key(const char *ns, const char *key)
{
    nvs_handle_t handle;
    if (nvs_open(ns, NVS_READWRITE, &handle) != ESP_OK)
        return false;

    if (nvs_erase_key(handle, key) != ESP_OK)
    {
        nvs_close(handle);
        return false;
    }

    nvs_commit(handle); 
    nvs_close(handle);
    return true;
}
