#include "mbed.h"
#include "hal/PpmOut.hpp"
#include "hal/i2c/bno055.hpp"
#include "hal/i2c/mpl3115a2.hpp"
#include "hal/i2c/srf02.hpp"

#include "defines.hpp"
#include "receiver.hpp"
#include "sensors.hpp"

DigitalOut ledRed(LED_RED);
DigitalOut ledGreen(LED_GREEN);
DigitalOut ledBlue(LED_BLUE);

PpmOut motor(MAIN_MOTOR);
PpmOut servoAileronRight(AILERON_RIGHT);
PpmOut servoAileronLeft(AILERON_LEFT);
PpmOut servoVTailRight(VTAIL_RIGHT);
PpmOut servoVTailLeft(VTAIL_LEFT);

Serial pc(USBTX, USBRX);

#define PITCH_P 1
#define ROLL_P 1

void _main()
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
    for(int c=0; c<80; c++){
        wait_ms(100);
        for(int c=0; c<16; c++)
            receiver::get(c);
    }

    // I²C und Sensor Fusion initialisieren
    if(!sensors::init()){
        while(true){
            ledRed = !ledRed;
            ledBlue = !ledBlue;
            ledGreen = !ledGreen;
        }
    }

    ledRed = LED_OFF;

    while (true) {
        if(receiver::get(6) > 800){
            ledBlue = LED_ON;
            ledRed = LED_ON;

            motor.setValue(receiver::get(0));

            servoAileronRight.setValue(500);
            servoAileronLeft.setValue(500);
            servoVTailRight.setValue(500);
            servoVTailLeft.setValue(500);
        } else if(receiver::get(6) > 300){
            ledBlue = LED_ON;
            ledRed = LED_OFF;
        }else{
            ledBlue = LED_OFF;
            ledRed = LED_OFF;

            motor.setValue(receiver::get(0));
            servoAileronRight.setValue(receiver::get(1));
            servoAileronLeft.setValue(receiver::get(2));
            servoVTailRight.setValue(receiver::get(5));
            servoVTailLeft.setValue(receiver::get(4));

            ledGreen = receiver::get(0) < 500;
        }
    }
}
