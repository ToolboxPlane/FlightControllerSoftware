#include "mbed.h"
#include "PpmOut.hpp"
#include "mpu9250Adapter.hpp"

#include "defines.hpp"
#include "receiver.hpp"

/*
    Debug Modes:
    0: DigitalOut Toggle Test
        Toggles all PPM, I²C and LEDs at 1Hz
    1: Receiver Test
        Sets the Leds to the first three channels of the SBus Receiver
    2: PPM Test
        Continously cycly through the PPM range
*/
#define _DEBUG_MODE 2

#if _DEBUG_MODE==0
    DigitalOut motor(MAIN_MOTOR);
    DigitalOut servoAileronRight(AILERON_RIGHT);
    DigitalOut servoAileronLeft(AILERON_LEFT);
    DigitalOut servoVTailRight(VTAIL_RIGHT);
    DigitalOut servoVTailLeft(VTAIL_LEFT);
    
    DigitalOut i2cSda(I2C_SDA);
    DigitalOut i2cScl(I2C_SCL);
    
    DigitalOut ledRed(LED_RED);
    DigitalOut ledGreen(LED_GREEN);
    DigitalOut ledBlue(LED_BLUE);
    
    void testMain(){    
        while(true){
            motor = !motor;
            servoAileronLeft = !servoAileronLeft;
            servoAileronRight = !servoAileronRight;
            servoVTailLeft = !servoVTailLeft;
            servoVTailRight = !servoVTailRight;
            i2cSda = !i2cSda;
            i2cScl = !i2cScl;
            
            ledBlue = !ledBlue;
            ledRed = !ledRed;
            ledGreen = !ledGreen;
            
            wait(1);
        }
    }
#elif _DEBUG_MODE==1
    AnalogOut ledRed(LED_RED);
    AnalogOut ledGreen(LED_GREEN);
    AnalogOut ledBlue(LED_BLUE);

    void testMain(){
        receiver::init();
        
        while(true){
            ledRed = receiver::get(0) / 1000.0;
            ledGreen = receiver::get(1) / 1000.0;
            ledBlue = receiver::get(2) / 1000.0;
        }
    }
#elif _DEBUG_MODE==2
    PpmOut motor(MAIN_MOTOR);
    PpmOut servoAileronRight(AILERON_RIGHT);
    PpmOut servoAileronLeft(AILERON_LEFT);
    PpmOut servoVTailRight(VTAIL_RIGHT);
    PpmOut servoVTailLeft(VTAIL_LEFT);
    
    DigitalOut ledGreen(LED_GREEN);
    
    void testMain(){
        motor.setValue(0);
        wait(10);
        while(true){
            for(int c=25; c<75; c++){
                motor.setValue(c*10);
                servoAileronRight.setValue(c*10);
                servoAileronLeft.setValue(c*10);
                servoVTailRight.setValue(c*10);
                servoVTailLeft.setValue(c*10);
                wait_ms(20);
            }
            for(int c=75; c<25; c--){
                motor.setValue(c*10);
                servoAileronRight.setValue(c*10);
                servoAileronLeft.setValue(c*10);
                servoVTailRight.setValue(c*10);
                servoVTailLeft.setValue(c*10);
                wait_ms(20);
            }
        }
    }
#else
    #error You need to select a _DEBUG_MODE
#endif