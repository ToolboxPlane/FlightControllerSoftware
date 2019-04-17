/**
 * @file setpoint.h
 * @author paul
 * @date 17.04.19
 * @brief setpoint @TODO
 */

#ifndef FLIGHTCONTROLLER_SETPOINT_H
#define FLIGHTCONTROLLER_SETPOINT_H

typedef struct {
    int16_t pitch, roll;
    uint16_t power;
} setpoint_t;

#endif //FLIGHTCONTROLLER_SETPOINT_H
