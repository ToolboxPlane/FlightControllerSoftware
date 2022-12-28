/**
 * @file protobuf.c
 * @author Paul Nykiel
 * @date 19.11.22
 * @brief Implementations of the library functions for protobuf encoding, transmission and reception.
 * @ingroup Drivers
 */
#include "protobuf.h"

#include <HAL/uart.h>
#include <Messages/MessageDecoding.h>
#include <Messages/MessageEncoding.h>
#include <Messages/MessageIds.h>

#include "ring_buffer.h"

enum { UART_ID = 0 };
enum { UART_BAUD = 115200U };

static ring_buffer_data_t ring_buffer_data;
static message_decoding_data_t message_decoding_data;
static setpoint_message_t setpoint_message;

static void uart_callback(uint8_t data) {
    if (!ring_buffer_put(&ring_buffer_data, data)) {
        // TODO error handling
    }
}

void protobuf_init(void) {
    message_decoding_init(&message_decoding_data, FC_SP_ID);
    ring_buffer_data = ring_buffer_init();
    uart_init(UART_ID, UART_BAUD, NONE, 1, uart_callback);
}

void protobuf_send(const fc_message_t *message) {
    static uint8_t buf[ToolboxPlaneMessages_FlightController_size + 3];
    uint16_t len = message_encode(buf, sizeof(buf), ToolboxPlaneMessages_FlightController_fields, message, FC_ID);
    uart_send_buf(UART_ID, buf, len);
}

bool protobuf_setpoint_available(void) {
    bool res = false;
    uint8_t data = 0;
    while (ring_buffer_get(&ring_buffer_data, &data)) {
        if (message_decoding_decode(&message_decoding_data, data, &ToolboxPlaneMessages_FlightControllerSetpoint_msg,
                                    &setpoint_message)) {
            res = true;
        }
    }
    return res;
}

setpoint_message_t protobuf_get_setpoint(void) {
    return setpoint_message;
}
