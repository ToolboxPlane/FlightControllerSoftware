/**
 * @file remote.h
 * @author Paul Nykiel
 * @date 13.11.22
 * @brief Declaration of the remote interface component.
 * @ingroup Components
 */
#ifndef FLIGHTCONTROLLER_REMOTE_H
#define FLIGHTCONTROLLER_REMOTE_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Struct containing all data received in a single remote packet.
 */
typedef struct {
    int16_t throttle_mixed;     ///< The mixed signals for the throttle, in [0,1000].
    int16_t elevon_left_mixed;  ///< The mixed signals for the left elevon , in [0,1000].
    int16_t elevon_right_mixed; ///< The mixed signals for the right elevon, in [0,1000].

    bool is_armed;        ///< True if the arm switch on the remote is set.
    bool override_active; ///< True if the override switch on the remote is set.

    /**
     * True if the last remote packet was ok, this means:
     *  * No failsave
     *  * No frame-lost
     */
    bool remote_ok;
} remote_data_t;

/**
 * @brief Initialize the remote module.
 *
 * The initialization consists of the following task:
 *  * call ::sbus_init
 */
void remote_init(void);

/**
 * Checks if new data is available.
 * @return the result of sbus_data_available
 */
bool remote_data_available(void);

/**
 * Gets the last package from the sbus module, normalizes the channel information to [0, 1000] and populates
 * the remote_data_t struct in accordance with the remote channel mapping.
 * @return the last package received
 */
remote_data_t remote_get_data(void);


#endif // FLIGHTCONTROLLER_REMOTE_H
