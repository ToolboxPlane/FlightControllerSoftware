#include "statemachine.h"

flightmode_t curr_flightmode;

void state_machine() {
    switch(curr_flightmode) {
        case MANUAL:
            manual_state_machine();
            break;
        case LAUNCH:
            launch_state_machine();
            break;
        case LAND:
            land_state_machine();
            break;
        case HOLD:
            hold_state_machine();
            break;
        case WAYPOINT:
            waypoint_state_machine();
            break;
    }
}

void launch_state_machine() {
    pitch_controller.enabled = true;
    heading_controller.enabled = false;
    roll_controller.enabled = true;

    pitch_controller.target_value = 10;
    roll_controller.target_value = 0;
}

void land_state_machine() {

}

void hold_state_machine() {

}

void waypoint_state_machine() {

}

void manual_state_machine() {

}
