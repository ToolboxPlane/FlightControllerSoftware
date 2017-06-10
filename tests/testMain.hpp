#include "mbed.h"
#include "hal/PpmOut.hpp"
#include "hal/SBus.hpp"

#include "hal/i2c/bno055.hpp"
#include "hal/i2c/mpl3115a2.hpp"
#include "hal/i2c/srf02.hpp"

#include "defines.hpp"
#include "receiver.hpp"

/*
    Debug Modes:
    0: DigitalOut Toggle Test
        Toggles all PPM, I²C and LEDs at 1Hz
    1: PPM Test
        Continously cycle through the PPM range
    2: SBus to Serial
        Log the first three channels of the sbus interface via the uart-interface
    3: SRF-02
        Log the distance recorded by the SRF-02 via the uart-interface
    4: MPL3115A2
        Log the height recorded by the sensor via the uart-interface
    5: Bno055
        Log the orientation in Euler Angles via the uart-interface
    6: I²C Scanner
*/
#define _DEBUG_MODE 0

#if _DEBUG_MODE==0
    DigitalOut motor(MAIN_MOTOR);
    DigitalOut servoAileronRight(AILERON_RIGHT);
    DigitalOut servoAileronLeft(AILERON_LEFT);
    DigitalOut servoVTailRight(VTAIL_RIGHT);
    DigitalOut servoVTailLeft(VTAIL_LEFT);

    DigitalOut i2cSda(D4);
    DigitalOut i2cScl(D5);

    DigitalOut led(LED1);

    void _main(){
        while(true){
            motor = !motor;
            servoAileronLeft = !servoAileronLeft;
            servoAileronRight = !servoAileronRight;
            servoVTailLeft = !servoVTailLeft;
            servoVTailRight = !servoVTailRight;
            i2cSda = !i2cSda;
            i2cScl = !i2cScl;

            led = !led;

            wait(1);
        }
    }
#elif _DEBUG_MODE==1
    PpmOut motor(MAIN_MOTOR);
    PpmOut servoAileronRight(AILERON_RIGHT);
    PpmOut servoAileronLeft(AILERON_LEFT);
    PpmOut servoVTailRight(VTAIL_RIGHT);
    PpmOut servoVTailLeft(VTAIL_LEFT);

    DigitalOut ledGreen(LED_GREEN);

    void _main(){
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
            for(int c=75; c>25; c--){
                motor.setValue(c*10);
                servoAileronRight.setValue(c*10);
                servoAileronLeft.setValue(c*10);
                servoVTailRight.setValue(c*10);
                servoVTailLeft.setValue(c*10);
                wait_ms(20);
            }
        }
    }
#elif _DEBUG_MODE==2
    Serial pc(USBTX, USBRX);

    void _main(){
        receiver::init();

        while(true){
            printf("Raw: %d\t%d\t%d\t Norm: %d\t%d\t%d\t Status: %d\tFL: %d\t FS: %d\n",
                receiver::sbus.getChannel(0), receiver::sbus.getChannel(1), receiver::sbus.getChannel(2),
                receiver::get(0), receiver::get(1), receiver::get(2), receiver::status(),
                receiver::sbus.frameLost(), receiver::sbus.failSave());
            wait(0.1);
        }
    }
#elif _DEBUG_MODE==3
    Serial pc(USBTX, USBRX);
    I2C i2c(I2C_SDA, I2C_SCL);
    Srf02 us(i2c);

    void _main(){
        printf("SRF-02 Test\n");

        us.startMeasurement();
        while(true){
            if(us.isReady()){
                printf("Dist %d\n", us.readDistance());
                us.startMeasurement();
            }

            /*if(!us.isAvailable()){
                printf("Sensor not available\n");
            }*/
            wait_ms(100);
        }
    }
#elif _DEBUG_MODE==4
    Serial pc(USBTX, USBRX);
    I2C i2c(I2C_SDA, I2C_SCL);
    Mpl3115a2 baro(i2c);

    void _main(){
        while(true){
            if(baro.isReady()){
                uint16_t alt = baro.getAltitude();
                printf("Height: %f [m]\t", alt);
            }
        }
    }
#elif _DEBUG_MODE==5
    Serial pc(USBTX, USBRX);
    I2C i2c(I2C_SDA, I2C_SCL);
    Bno055 imu(i2c);

    void _main(){
        while(true){
            int16_t eulHeading, eulPitch, eulRoll;

            eulHeading = imu.eulHeading();
            eulPitch = imu.eulPitch();
            eulRoll = imu.eulRoll();

            printf("IMU: %d\t%d\t%d\n", eulHeading, eulPitch, eulRoll);
        }
    }
#elif _DEBUG_MODE==6

    I2C i2c(I2C_SDA , I2C_SCL );

    void _main() {
        printf("\nI2C Scanner");

        while(1) {
            int error, address;
            int nDevices;

            printf("Scanning...\n");

             nDevices = 0;

              for(address = 1; address < 127; address++ )
              {
                i2c.start();
                error = i2c.write(address << 1); //We shift it left because mbed takes in 8 bit addreses
                i2c.stop();
                if (error == 1)
                {
                  printf("I2C device found at address 0x%X aka 0x%X\n", address, address<<1);
                  nDevices++;
                }

              }
              if (nDevices == 0)
                printf("No I2C devices found\n");
              else
                printf("\ndone\n");

              wait(5);           // wait 5 seconds for next scan

            }
    }
#else
    #error You need to select a _DEBUG_MODE
#endif
