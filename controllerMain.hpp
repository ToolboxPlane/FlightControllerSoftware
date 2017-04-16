#include "mbed.h"
#include "hal/PpmOut.hpp"
#include "hal/i2c/bno055.hpp"
#include "hal/i2c/mpl3115a2.hpp"
#include "hal/i2c/srf02.hpp"

#include "defines.hpp"
#include "receiver.hpp"
#include "sensors.hpp"
#include "autonomous.hpp"

#define MIN(x,y) (x<y?x:y)

DigitalOut ledRed(LED_RED);
DigitalOut ledGreen(LED_GREEN);
DigitalOut ledBlue(LED_BLUE);

Serial pc(USBTX, USBRX);

uint8_t landingStage = 0;

void _main()
{
    // LEDs aus, Rot an
    ledRed = LED_ON;
    ledGreen = LED_OFF;
    ledBlue = LED_OFF;

    // Receiver Adapter initialisieren
    receiver::init();

    // ESC Initialisieren und Fernbedienungsoffsets lesen
    flightControl::init();

    // I²C und Sensor Fusion initialisieren
    while(!sensors::init()){
        ledRed = !ledRed;
        ledBlue = !ledBlue;
        ledGreen = !ledGreen;
        wait_ms(500);
    }

    ledRed = LED_OFF;

    while (true) {
        //printf("FL: %d\tFS: %d\tBC: %d\n", receiver::sbus.frameLost(), receiver::sbus.failSave(), receiver::sbus.badConnection());
        if(!receiver::connected()){
            sensors::heightRequired = false;
            sensors::imuRequired = true;

            levelFlight(0, -5, 0);
            ledGreen = LED_ON;
            ledRed = LED_ON;
            ledBlue = LED_ON;
        } else {
            //printf("%d\t%d\t%d\t%d\n", sensors::height, sensors::heading, sensors::pitch, sensors::roll);

            if(receiver::get(6) > 800){
                ledBlue = LED_OFF;
                ledRed = LED_ON;
                ledGreen = LED_OFF;
                sensors::heightRequired = true;
                sensors::imuRequired = true;

                if(landingStage == 1){
                    levelFlight(0, 10, 0);
                } else if(sensors::heightSource == BARO){
                    levelFlight(0, -12, 120);
                    landingStage = 2;
                } else if(sensors::height > 50){
                    levelFlight(0, -8, 100);
                    landingStage = 2;
                } else if(sensors::height < 40){
                    landingStage = 1;
                }

            } else if(receiver::get(6) > 300){
                landingStage = 3;
                ledBlue = LED_ON;
                ledRed = LED_OFF;
                ledGreen = LED_OFF;
                sensors::heightRequired = false;
                sensors::imuRequired = true;

                levelFlight(0, 0, receiver::get(0));
            } else {
                landingStage = 3;
                ledBlue = LED_OFF;
                ledRed = LED_OFF;
                ledGreen = LED_ON;
                sensors::heightRequired = false;
                sensors::imuRequired = false;

                flightControl::motor.setValue(receiver::get(0));
                flightControl::servoAileronRight.setValue(receiver::get(1));
                flightControl::servoAileronLeft.setValue(receiver::get(2));
                flightControl::servoVTailRight.setValue(receiver::get(5));
                flightControl::servoVTailLeft.setValue(receiver::get(4));
            }
        }
    }
}
