#ifndef FLIGHTCONTROLLER_RCLIB_H
#define FLIGHTCONTROLLER_RCLIB_H

#include <stdint.h>
#include <stdbool.h>

#define RC_LIB_START 0xC9 //201_{10}
#define  RC_LIB_END 0x93 //147_{10}
#define DATA_BUFFER_SIZE 64

extern uint8_t rc_lib_global_package_uid;
extern uint8_t rc_lib_transmitter_id;
extern uint8_t rc_lib_error_count;

typedef struct rc_lib_package {
    uint16_t channel_data[DATA_BUFFER_SIZE];
    uint8_t buffer[DATA_BUFFER_SIZE];
    uint8_t buf_count;

    uint8_t uid; ///< Unique (package) id
    uint8_t tid; ///< Unique transmitter (device) id
    uint16_t channel_count; ///< Number of saved channels
    uint16_t resolution; ///< Resolution of each channel in pixels

    uint8_t checksum;

    uint8_t mesh;
    uint8_t routing_length;
    uint8_t discover_state; ///< Zero means no discovery-message, one is a discovery message, two a discovery response
} rc_lib_package_t;

uint8_t rc_lib_encode(rc_lib_package_t* package);
uint8_t rc_lib_decode(rc_lib_package_t* package, uint8_t data);
uint8_t rc_lib_calculate_checksum(rc_lib_package_t* checksum);
uint8_t is_discover_message(rc_lib_package_t* package);
uint8_t is_discover_response(rc_lib_package_t* package);
void set_discover_message(rc_lib_package_t* package);
void make_discover_response(rc_lib_package_t* new_package, rc_lib_package_t* responses, uint8_t len);

uint8_t _rc_lib_resolution_steps_2_key(uint16_t steps);
uint8_t _rc_lib_channel_count_2_key(uint16_t channel_count);
uint8_t _rc_lib_resolution_steps_2_bit_count(uint16_t steps);
uint16_t _rc_lib_key_2_resolution_steps(uint8_t key);
uint16_t _rc_lib_key_2_channel_count(uint8_t key);

#endif //FLIGHTCONTROLLER_RCLIB_H
