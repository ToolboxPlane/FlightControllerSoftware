/**
 * @file remote.h
 * @author paul
 * @date 13.11.22
 * Description here TODO
 */
#ifndef FLIGHTCONTROLLER_REMOTE_H
#define FLIGHTCONTROLLER_REMOTE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int16_t throttle_mixed, elevon_left_mixed, elevon_right_mixed;
    int16_t throttle_raw, pitch_raw, roll_raw;
    bool is_armed;
    bool override_active;
    bool remote_ok;
} remote_data_t;

void remote_init(void);


#endif // FLIGHTCONTROLLER_REMOTE_H
