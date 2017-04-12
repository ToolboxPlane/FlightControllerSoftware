#ifndef _BNO_055_H
#define _BNO_055_H

#include "mbed.h"

enum Bno055Mode{
    CONFIGMODE = 0b0000,
    ACCONLY = 0b0001,
    MAGONLY = 0b0010,
    GYROONLY = 0b0011,
    ACCMAG = 0b0100,
    ACCGYRO = 0b0101,
    MAGGYRO = 0b0110,
    AMG = 0b0111,
    IMU = 0b1000,
    COMPASS = 0b1001,
    M4G = 0b1010,
    NDOF_FMC_OFF = 0b1011,
    NDOF = 0b1100
};

enum AccUnit{
    MS2 = 0,
    MG = 1
};

enum AngularRate{
    DPS = 0,
    RPS = 1
};

enum EulerAngles{
    DEGREES = 0,
    RADIANS = 1
};

enum TemperatureUnit{
    CELSIUS = 0,
    FAHRENHEIT = 1
};

enum OutputFormat{
    WINDOWS = 0,
    ANDROID = 1
};

enum SystemStatus{
    SYSTEM_IDLE = 0,
    SYSTEM_ERROR = 1,
    INITIALIZING_PERIPHERALS = 2,
    SYSTEM_INITIALIZATION = 3,
    EXECUTING_SELFTEST = 4,
    SENSOR_FUSION_ALGORITHM_RUNNING = 5,
    SYSTEM_RUNNING_WITHOUT_FUSION_ALGORITHM = 6
};

class Bno055{
    public:
        Bno055(I2C& i2c, Bno055Mode mode = NDOF_FMC_OFF,
                        AccUnit accUnit=MS2,
                        AngularRate angularRate=DPS,
                        EulerAngles eulerAngles=DEGREES,
                        TemperatureUnit temperatureUnit=CELSIUS,
                        OutputFormat outputFormat=WINDOWS,
                        uint8_t addr = 0x50);

        // Status ...
        uint8_t getDeviceId();
        uint8_t getStatus();
        uint8_t isAvailable();

        //Beschleunigungs Vektoren
		int16_t accDataX(void);
		int16_t accDataY(void);
		int16_t accDataZ(void);

		//Magnetfeld Vektoren
		int16_t magDataX(void);
		int16_t magDataY(void);
		int16_t magDataZ(void);

		//Gyro Vektoren
		int16_t gyrDataX(void);
		int16_t gyrDataY(void);
		int16_t gyrDataZ(void);

		//Ausrichtung in Euler-Winkeln
		int16_t eulHeading(void);
		int16_t eulRoll(void);
		int16_t eulPitch(void);

		//Ausrichtung in Quaternionen
		float quaDataW(void);
		float quaDataX(void);
		float quaDataY(void);
		float quaDataZ(void);

		//Lineare Beschleunigungs Vektoren
		float liaDataX(void);
		float liaDataY(void);
		float liaDataZ(void);

		//Gravitations Vektoren
		float grvDataX(void);
		float grvDataY(void);
		float grvDataZ(void);

    private:
        I2C i2c;
        uint8_t addr;
        int16_t getWord(uint8_t reg);
};

#endif
