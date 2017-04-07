#include "mbed.h"
#include "hal/PpmOut.hpp"

#include "defines.hpp"
#include "receiver.hpp"

DigitalOut ledRed(LED_RED);
DigitalOut ledGreen(LED_GREEN);
DigitalOut ledBlue(LED_BLUE);

PpmOut motor(MAIN_MOTOR);
PpmOut servoAileronRight(AILERON_RIGHT);
PpmOut servoAileronLeft(AILERON_LEFT);
PpmOut servoVTailRight(VTAIL_RIGHT);
PpmOut servoVTailLeft(VTAIL_LEFT);

Serial pc(USBTX, USBRX);

void controllerMain()
{
    // LEDs aus, Rot an
    ledRed = LED_ON;
    ledGreen = LED_OFF;
    ledBlue = LED_OFF;

    // Servos auf Mittelstellung
    servoAileronRight.setValue(500);
    servoAileronLeft.setValue(500);
    servoVTailRight.setValue(500);
    servoVTailLeft.setValue(500);

    // Receiver Adapter initialisieren
    receiver::init();

    // ESC Initialisieren und Fernbedienungsoffsets lesen
    motor.setValue(0);
    for(int c=0; c<100; c++){
        wait_ms(100);
        for(int c=0; c<16; c++)
            receiver::get(c);
    }

    ledRed = LED_OFF;

    while (true) {
        if(receiver::get(5)){
            ledBlue = LED_ON;

            // @TODO Autonomous mode
        }else{
            ledBlue = LED_OFF;

            // @TODO Channel
            motor.setValue(receiver::get(0));
            servoAileronRight.setValue(receiver::get(1));
            servoAileronLeft.setValue(receiver::get(2));
            servoVTailRight.setValue(receiver::get(3));
            servoVTailLeft.setValue(receiver::get(4));

            ledGreen = receiver::get(0) > 500;
        }
    }
}
