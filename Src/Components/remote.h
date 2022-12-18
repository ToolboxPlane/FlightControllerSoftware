/**
 * @file remote.h
 * @author Paul Nykiel
 * @date 13.11.22
 * @brief Declaration of the remote interface component.
 */
#ifndef FLIGHTCONTROLLER_REMOTE_H
#define FLIGHTCONTROLLER_REMOTE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int16_t throttle_mixed, elevon_left_mixed, elevon_right_mixed;
    int16_t throttle_raw, pitch_raw, roll_raw;
    bool is_armed;
    bool override_active;
    bool remote_ok;
} remote_data_t;

void remote_init(void);

remote_data_t remote_get_data(void);

bool remote_data_available(void);

#endif // FLIGHTCONTROLLER_REMOTE_H
