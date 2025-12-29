// midi_queue.c
#include "midi_queue.h"
#include <stdio.h>
#include "driver/uart.h"

// handle task and Queur
static QueueHandle_t midiQueueHandle = NULL;
static TaskHandle_t midiTaskHandle = NULL;

/* -----------------------------
   UART INIT (MIDI over USB)
-------------------------------- */
static void midi_uart_init()
{
    const uart_config_t uart_config = {
        .baud_rate = 115200, // MIDI over USB serial
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};

    uart_param_config(UART_NUM_0, &uart_config);
    uart_driver_install(UART_NUM_0, 1024, 0, 0, NULL, 0);
}

/* -----------------------------
   SEND MIDI BYTES
-------------------------------- */

/* -------------------------------------
  Status - 0x90(on) 0x80(off) + channel
    |
  Note
    |
  Velocity
---------------------------------------- */

// byte in UART
static void midi_send_byte(uint8_t byte)
{
    uart_write_bytes(UART_NUM_0, (const char *)&byte, 1);
}

// note on
static void midi_send_note_on(uint8_t note, uint8_t velocity)
{
    midi_send_byte(0x90);
    midi_send_byte(note);
    midi_send_byte(velocity);
}

// note off
static void midi_send_note_off(uint8_t note)
{
    midi_send_byte(0x80);
    midi_send_byte(note);
    midi_send_byte(0);
}

/* -----------------------------
   MANAGE QUEUE
-------------------------------- */

QueueHandle_t create_midi_queue()
{
    if (!midiQueueHandle)
    {
        midiQueueHandle = xQueueCreate(QUEUE_SIZE, sizeof(midi_event_t));
    }
    return midiQueueHandle;
}

QueueHandle_t get_midi_queue()
{
    return midiQueueHandle;
}

void delete_midi_queue()
{
    if(midiTaskHandle)
    {
        vTaskDelete(midiTaskHandle);
        midiTaskHandle = NULL;
    }
    if (midiQueueHandle)
    {
        vQueueDelete(midiQueueHandle);
        midiQueueHandle = NULL;
    }
}

// Task consumer + queue
static void midi_task(void *arg)
{
    midi_event_t event;
    while (1)
    {
        if (xQueueReceive(midiQueueHandle, &event, portMAX_DELAY) == pdTRUE)
        {

            if (!event.noteOff)
            {
                printf("MIDI Note On: %d, velocity: %d\n", event.note, event.velocity);
                // midi_send_note_on(event.note, event.velocity);
            }

            else
            {
                // printf("MIDI Note Off: %d\n", event.note);
                // midi_send_note_off(event.note);
            }
        }
    }
}

// Start Task
void start_queue_midi_task(int cpuCore)
{
    if (!midiTaskHandle)
    {
        midi_uart_init();
        if (midiQueueHandle)
        {
            xTaskCreatePinnedToCore(
                midi_task,   // func
                "midi_task", // name
                2048,        // stack
                NULL,        // params
                12,           // pririty
                &midiTaskHandle, // handle
                cpuCore            // CPU core (0 || 1)
            );
        }
    }
}
