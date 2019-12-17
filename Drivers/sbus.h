//
// Created by paul on 14.04.18.
//

#ifndef FLIGHTCONTROLLER_SBUS_H
#define FLIGHTCONTROLLER_SBUS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint16_t channel[16];
    bool failsave, frame_lost;
}sbus_data_t;

bool sbus_parse(const uint8_t *data, uint8_t len);

extern sbus_data_t sbus_latest_data;

#endif //FLIGHTCONTROLLER_SBUS_H
