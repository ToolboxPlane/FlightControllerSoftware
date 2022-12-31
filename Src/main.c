/**
 * @file main.c
 * @author Paul Nykiel
 * @date 12.04.19
 * @brief Main file of the Flightcontroller firmware.
 */
#include "Application/application.h"

/**
 * @defgroup HAL Hardware-Abstraction-Layer
 * @brief Drivers for the different CPU peripherals.
 */

/**
 * @defgroup Drivers Drivers-Layer
 * @brief Drivers for the different sensors and communication interfaces.
 */

/**
 * @defgroup Components Components-Layer
 * @brief Library for the application specific communication with sensors and interfaces.
 */

/**
 * @defgroup Application Application-Library
 * @brief Library for application specific functions.
 */

/**
 * Main function, all functionality is extracted to application for testing purposes.
 * @return nothing
 */
int main(void) {
    application_init();
}
