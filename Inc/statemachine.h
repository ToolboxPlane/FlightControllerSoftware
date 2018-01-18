//
// Created by paul on 01.01.18.
//

#include "controller.h"

#ifndef FLIGHTCONTROLLER_STATEMACHINE_H
#define FLIGHTCONTROLLER_STATEMACHINE_H

typedef enum flightmode {
    MANUAL, LAUNCH, LAND, HOLD, WAYPOINT
}flightmode_t;

void state_machine();
extern flightmode_t curr_flightmode;

void manual_state_machine();
void launch_state_machine();
void land_state_machine();
void hold_state_machine();
void waypoint_state_machine();

#endif //FLIGHTCONTROLLER_STATEMACHINE_H
