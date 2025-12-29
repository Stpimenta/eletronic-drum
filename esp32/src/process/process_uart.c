#include "process_uart.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>
#include "controller_uart/pad_controller.h"
#include "controller_uart/engine_controller.h"

#define UART_BUF_SIZE 3096
static uart_port_t s_uart_num;
static uint8_t uart_rx_buf[UART_BUF_SIZE];
static char engine_resp_buf[2048];
static TaskHandle_t s_uart_task_handle = NULL;

static void uart_task(void *arg)
{
    uint8_t *buf = uart_rx_buf;

    while (1)
    {
        int len = uart_read_bytes(
            s_uart_num,
            buf,
            UART_BUF_SIZE - 1,
            pdMS_TO_TICKS(1000));

        if (len <= 0)
            continue;

        buf[len] = 0;

        // printf("\n[UART RX] len=%d\n", len);
        // printf(">> RAW:\n%s\n", (char *)buf);

        bool all_ok = true;
        bool suppress_ack = false;

        char *saveptr;
        char *line = strtok_r((char *)buf, "\n", &saveptr);

        while (line)
        {
            if (*line != '\0')
            {
                size_t l = strlen(line);
                if (l > 0 && line[l - 1] == '\r')
                    line[l - 1] = '\0';

                if (strcmp(line, "GET ENGINE") == 0)
                    suppress_ack = true;

                if (!protocol_dispatch(line))
                    all_ok = false;
            }

            line = strtok_r(NULL, "\n", &saveptr);
        }

        if (!suppress_ack)
        {
            const char *resp = all_ok ? "ACK\n" : "ERR\n";
            uart_write_bytes(s_uart_num, resp, strlen(resp));
        }
    }
}



bool protocol_dispatch(const char *cmd)
{
    if (!cmd || strlen(cmd) < 3)
        return false;

    // ---- GET ENGINE ----
    if (strcmp(cmd, "GET ENGINE") == 0)
    {
        char *resp = engine_resp_buf;
        resp[0] = '\0';


        if (engine_controller_get_all(resp, 2048))
        {
            uart_write_bytes(s_uart_num, resp, strlen(resp));
            return true;
        }
        return false;

        // char resp[2048];
        // Consegue conter, por exemplo:
        // 30 pads com linhas desse tamanho:
        // PAD 1 snare 100 38 4095 20 50\n   // ~33 bytes
        // O limite real não é só o buffer, é também:
        // UART
        // stack da task (uart_task = 3072 bytes)
    }

    // ---- SET ----
    if (strncmp(cmd, "SET", 3) == 0)
    {
        return protocol_dispatch_set(cmd);
    }

    return false;
}

bool protocol_dispatch_set(const char *cmd)
{
    if (strstr(cmd, " PAD ") != NULL)
    {
        return pad_controller_handle_set(cmd);
    }

    if (strstr(cmd, " SNARE ") != NULL)
    {
        return false;
    }

    return false;
}


void start_process_uart(uart_port_t uart_num, int rx_pin, int tx_pin, int cpu_core)
{
    s_uart_num = uart_num;
    uart_config_t cfg = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};

    uart_driver_install(uart_num, UART_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(uart_num, &cfg);
    uart_set_pin(uart_num, tx_pin, rx_pin,
                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

     xTaskCreatePinnedToCore(
        uart_task,        // função da task
        "uart_process",   // nome
        4096,             // stack size
        NULL,             // parâmetro
        5,               // prioridade
        &s_uart_task_handle, // handle
        cpu_core                // core (0 ou 1)
    );
}

void stop_process_uart(void)
{
    // 1. Deleta a task
    if (s_uart_task_handle != NULL)
    {
        vTaskDelete(s_uart_task_handle);
        s_uart_task_handle = NULL;
    }
    // 2. Desinstala o driver UART
    uart_driver_delete(s_uart_num);

    // 3. Limpa estado global
    s_uart_num = UART_NUM_MAX; 
    memset(uart_rx_buf, 0, sizeof(uart_rx_buf));
    memset(engine_resp_buf, 0, sizeof(engine_resp_buf));
}