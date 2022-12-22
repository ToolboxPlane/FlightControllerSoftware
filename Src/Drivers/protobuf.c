/**
 * @file protobuf.c
 * @author Paul Nykiel
 * @date 19.11.22
 * @brief Implementations of the library functions for protobuf encoding, transmission and reception.
 */
#include "protobuf.h"

#include <HAL/uart.h>
#include <Messages/MessageDecoding.h>
#include <Messages/MessageEncoding.h>
#include <Messages/MessageIds.h>

enum { UART_ID = 0 };
enum { UART_BAUD = 115200U };

enum { RX_BUF_SIZE = 512 };

static volatile uint8_t rx_buf[RX_BUF_SIZE];
static volatile uint8_t rx_head, rx_tail;
static message_decoding_data_t message_decoding_data;
static setpoint_message_t setpoint_message;

static void uart_callback(uint8_t data) {
    rx_buf[rx_head] = data;
    rx_head = (rx_head + 1) % RX_BUF_SIZE;
    if (rx_tail == rx_head) {
        // TODO error handling
    }
}

void protobuf_init(void) {
    uart_init(UART_ID, UART_BAUD, NONE, 1, uart_callback);
    rx_head = 0;
    rx_tail = 0;
    message_decoding_data.len = 0;
    message_decoding_data.id = FC_SP_ID;
    message_decoding_data.decodingState = DECODING_INITIAL;
}

void protobuf_send(const fc_message_t *message) {
    static uint8_t buf[ToolboxPlaneMessages_FlightController_size + 3];
    uint16_t len = message_encode(buf, sizeof(buf), ToolboxPlaneMessages_FlightController_fields, message, FC_ID);
    uart_send_buf(UART_ID, buf, len);
}

bool protobuf_setpoint_available(void) {
    pb_istream_t istream;
    bool res = false;
    while (rx_tail != rx_head) {
        uint8_t data = rx_buf[rx_tail];
        rx_tail = (rx_tail + 1) % RX_BUF_SIZE;

        if (message_decode(&message_decoding_data, data, &istream)) {
            pb_decode(&istream, &ToolboxPlaneMessages_FlightControllerSetpoint_msg, &setpoint_message);
            res = true;
        }
    }
    return res;
}

setpoint_message_t protobuf_get_setpoint(void) {
    return setpoint_message;
}
