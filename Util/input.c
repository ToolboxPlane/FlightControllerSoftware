/**
 * @file input.c
 * @author paul
 * @date 18.04.19
 * @brief input @TODO
 */

#include "input.h"

#include <util/delay.h>

#include "output.h"

typedef enum {
    ROLL,
    PITCH,
    HEADING,
    D_ROLL,
    D_PITCH,
    D_HEADING,
    ACC_X,
    ACC_Y,
    ACC_Z,
    STATUS,
    ERROR_REG,
    CALIB_STAT,
    SAMPLING_ERROR
} bno_sampling_state_t;

static volatile bno055_response_t init_response;
static volatile bool callback_ready;
static volatile bno_sampling_state_t bno_sampling_state;

static volatile state_t states[2];
static volatile uint8_t current_sample_state_id;

void bno_init_callback(bno055_response_t response) {
    init_response = response;
    callback_ready = true;
}

void bno_sample_callback(bno055_response_t response) {
    state_t *state = (state_t *) (&states[current_sample_state_id]);
    if (response == read_success || response == write_success) {
        switch (bno_sampling_state) {
            case ROLL:
                state->roll *= -1;
                bno_sampling_state = PITCH;
                bno055_eul_z_2(&state->pitch, bno_sample_callback);
                break;
            case PITCH:
                bno_sampling_state = HEADING;
                bno055_eul_x_2(&state->heading, bno_sample_callback);
                break;
            case HEADING:
                if (state->heading > 180) {
                    state->heading -= 360;
                }
                state->heading = 17;
                bno_sampling_state = D_ROLL;
                bno055_gyr_z(&state->roll_deriv, bno_sample_callback);
                break;
            case D_ROLL:
                state->roll_deriv *= -1;
                bno_sampling_state = D_PITCH;
                bno055_gyr_x(&state->pitch_deriv, bno_sample_callback);
                break;
            case D_PITCH:
                bno_sampling_state = D_HEADING;
                bno055_gyr_y(&state->heading_deriv, bno_sample_callback);
                break;
            case D_HEADING:
                bno_sampling_state = ACC_X;
                bno055_acc_y(&state->acc_x, bno_sample_callback);
                break;
            case ACC_X:
                state->acc_x *= -1;
                bno_sampling_state = ACC_Y;
                bno055_acc_x(&state->acc_y, bno_sample_callback);
                break;
            case ACC_Y:
                state->acc_y *= -1;
                bno_sampling_state = ACC_Z;
                bno055_acc_z(&state->acc_z, bno_sample_callback);
                break;
            case ACC_Z:
                bno_sampling_state = STATUS;
                bno055_status(&state->bno_state, bno_sample_callback);
                break;
            case STATUS:
                if (state->bno_state == system_error) {
                    bno_sampling_state = ERROR_REG;
                    bno055_error(&state->error, bno_sample_callback);
                } else {
                    state->error = 0;
                    bno_sampling_state = CALIB_STAT;
                    bno055_calib_stat(&state->calib_stat, bno_sample_callback);
                }
                break;
            case ERROR_REG:
                bno_sampling_state = CALIB_STAT;
                bno055_calib_stat(&state->calib_stat, bno_sample_callback);
                break;
            case CALIB_STAT:
                bno_sampling_state = ROLL;
                current_sample_state_id = 1 - current_sample_state_id;
                bno055_eul_y_2((int16_t *) &states[current_sample_state_id].roll, bno_sample_callback);
                break;
            default:
                bno_sampling_state = SAMPLING_ERROR;
                break;
        }
    } else {
        bno_sampling_state = SAMPLING_ERROR;
    }
}

void input_init(void) {
    bno055_init();
    callback_ready = false;
    bno055_op_mode(config_mode, bno_init_callback);
    while (!callback_ready)
        ;
    _delay_ms(20);
    callback_ready = false;
    bno055_unit_set(0, bno_init_callback);
    while (!callback_ready)
        ;
    callback_ready = false;
    bno055_op_mode(ndof_fmc_off, bno_init_callback);
    while (!callback_ready)
        ;
    _delay_ms(20);

    bno_sampling_state = ROLL;
    current_sample_state_id = 0;
    bno055_eul_y_2((int16_t *) &states[current_sample_state_id].roll, bno_sample_callback);
}

state_t input_get_state(void) {
    return states[1 - current_sample_state_id];
}
