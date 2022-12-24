/**
 * @file mode_handler.h
 * @author Paul Nykiel
 * @date 21.12.22
 * @brief Declaration of the mode handler application.
 * @ingroup Application
 */
#ifndef FLIGHTCONTROLLER_MODE_HANDLER_H
#define FLIGHTCONTROLLER_MODE_HANDLER_H

#include "Components/flightcomputer.h"
#include "Components/imu.h"
#include "Components/remote.h"

typedef enum { MODE_FLIGHTCOMPUTER, MODE_REMOTE, MODE_STABILISED_FAILSAVE, MODE_FAILSAVE } mode_handler_mode_t;

enum {
    MODE_HANDLER_ERROR_NO_IMU_DATA = 1,
    MODE_HANDLER_ERROR_NO_FCP_DATA = 2,
    MODE_HANDLER_ERROR_NO_REMOTE_DATA = 3,
};


mode_handler_mode_t mode_handler_handle(imu_data_t *imu_data, remote_data_t *remote_data,
                                        flightcomputer_setpoint_t *flightcomputer_setpoint);

#endif // FLIGHTCONTROLLER_MODE_HANDLER_H
