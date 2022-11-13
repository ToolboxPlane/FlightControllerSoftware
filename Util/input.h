/**
 * @file input.h
 * @author paul
 * @date 18.04.19
 * @brief input @TODO
 */

#include "../state.h"

#ifndef FLIGHTCONTROLLER_INPUT_H
#define FLIGHTCONTROLLER_INPUT_H

#include <stdbool.h>

bool input_init(void);
void input_start_sampling(void);
state_t input_get_state(void);

#endif //FLIGHTCONTROLLER_INPUT_H
