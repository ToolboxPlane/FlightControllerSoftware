/**
 * @file input.h
 * @author paul
 * @date 18.04.19
 * @brief input @TODO
 */

#include "../state.h"

#ifndef FLIGHTCONTROLLER_INPUT_H
#define FLIGHTCONTROLLER_INPUT_H

void input_init(void);
void input_get_state(volatile state_t *state);

#endif //FLIGHTCONTROLLER_INPUT_H
