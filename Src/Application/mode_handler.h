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

/**
 * Errors reported by the mode handler to the error handler component.
 */
enum {
    MODE_HANDLER_ERROR_NO_IMU_DATA = 1,
    MODE_HANDLER_ERROR_NO_FCP_DATA = 2,
    MODE_HANDLER_ERROR_NO_REMOTE_DATA = 3,
};

/**
 * Possible flight modes depending on availability and integrity of data.
 */
typedef enum { MODE_FLIGHTCOMPUTER, MODE_REMOTE, MODE_STABILISED_FAILSAVE, MODE_FAILSAVE } mode_handler_mode_t;

/**
 * Initialize the mode handler by setting the timeouts for all devices to true.
 */
void mode_handler_init(void);

/**
 * @brief Query all components for new data, check the timeouts and whether the data is ok, the mode is then returned
 * based on the availability.
 *
 * First the timeout is checked for all devices, a device is set to not have a timeout if [device]_data_available()
 * return true at least once in the last [device]_TIMEOUT calls.
 * Next the integrity of the data is checked, a device is set to be active if it has no timeout and
 * [device]_get_data().[device]_ok. For every non-active device error_handle_warning(MODE_HANDLER,
 * MODE_HANDLER_ERROR_NO_[device]_DATA) is called.
 *
 * Depending on the active-status of all devices and the remote.override_active and remote.armed the mode is decided
 * according to the following decision table:
 *
 * | IMU | Remote | FCP | Override | Armed | Mode                 |
 * | --- | ------ | --- | -------- | ----- | -------------------- |
 * |  Y  |   Y    |  Y  |    N     |   Y   | FCP                  |
 * |     |   Y    |     |          |       | Remote               |
 * |  Y  |   N    |     |          |       | Stabilisied Failsave |
 * |  N  |   N    |     |          |       | Failsave             |
 *
 *
 * @param imu_data out-parameter, contains the last valid imu measurement
 * @param remote_data out-parameter, contains the last valid remote package
 * @param flightcomputer_setpoint out-parameter, contains the last valid flightcomputer package
 * @return returns the mode based on the availability of the individual packages
 */
mode_handler_mode_t mode_handler_handle(imu_data_t *imu_data, remote_data_t *remote_data,
                                        flightcomputer_setpoint_t *flightcomputer_setpoint);

#endif // FLIGHTCONTROLLER_MODE_HANDLER_H
