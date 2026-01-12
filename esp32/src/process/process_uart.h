#include <hal/uart_types.h>
#pragma once
void start_process_uart(uart_port_t uart_num, int rx_pin, int tx_pin, int cpu_core);
bool protocol_dispatch(const char *cmd);
bool protocol_dispatch_set(const char *cmd);