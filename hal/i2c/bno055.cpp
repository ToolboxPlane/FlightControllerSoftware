#include "bno055.hpp"

Bno055::Bno055(I2C &_i2c, Bno055Mode mode,
                AccUnit accUnit,
                AngularRate angularRate,
                EulerAngles eulerAngles,
                TemperatureUnit temperatureUnit,
                OutputFormat outputFormat,
                uint8_t addr) : i2c(_i2c){

    this->addr = addr;

    // Reset @CHECK necessary
    char cmd[2] = {0x3F, 0b1<<5};    //SYS_TRIGGER, RST_SYS
    i2c.write(addr, cmd, 2);

    wait(1);

    i2c.stop();

    // Set units
    cmd[0] = 0x3B; //UNIT_SEL
    cmd[1] = accUnit << 0 | angularRate << 1 | eulerAngles << 2
            | temperatureUnit << 4 | outputFormat << 7;
    i2c.write(addr, cmd, 2);

    // Enter OP-Mode
    cmd[0] = 0x3D;  //OPR_MODE
    cmd[1] = mode;
    i2c.write(addr, cmd, 2);

    wait_ms(100);
}

uint8_t Bno055::getDeviceId(){
    char cmd[] = {0x00}; // CHIP_ID
    char res[1];

    i2c.write(addr, cmd, 1);
    i2c.read(addr, res, 1);

    return res[0];
}

uint8_t Bno055::getStatus(){
    char cmd[] = {0x39}; // SYS_STATUS
    char res[1];

    i2c.write(addr, cmd, 1);
    i2c.read(addr, res, 1);

    return res[0];
}

uint8_t Bno055::isAvailable(){
    return getDeviceId() ==  0xA0;
}

int16_t Bno055::getWord(uint8_t reg){
    char res[2];
    char cmd[] = {reg};

    i2c.write(addr, cmd, 1);
    i2c.read(addr, res, 2);

    return ((int16_t)res[1] << 8) | res[0];
}

//Beschleunigungs Vektoren
int16_t Bno055::accDataX(void)
{
	return this->getWord(0x8);
}
int16_t Bno055::accDataY(void)
{
	return this->getWord(0xA);
}
int16_t Bno055::accDataZ(void)
{
	return this->getWord(0xC);
}
//Magnetfeld Vektoren
int16_t Bno055::magDataX(void)
{
	return this->getWord(0xE);
}
int16_t Bno055::magDataY(void)
{
	return this->getWord(0x10);
}
int16_t Bno055::magDataZ(void)
{
	return this->getWord(0x12);
}
//Gyro Vektoren
int16_t Bno055::gyrDataX(void)
{
	return this->getWord(0x14);
}
int16_t Bno055::gyrDataY(void)
{
	return this->getWord(0x16);
}
int16_t Bno055::gyrDataZ(void)
{
	return this->getWord(0x18);
}
//Ausrichtung in Euler-Winkeln
int16_t Bno055::eulHeading(void)
{
    int16_t angle = (int16_t)(this->getWord(0x1A)/16);
    angle %= 360;

    if(angle > 180)
        angle -= 360;

	return angle;
}
int16_t Bno055::eulRoll(void)
{
	int16_t angle = (int16_t)(this->getWord(0x1C)/16);

    angle %= 360;

    if(angle > 180)
        angle -= 360;

	return angle;
}
int16_t Bno055::eulPitch(void)
{
	int16_t angle = (int16_t)(this->getWord(0x1E)/16);

    angle %= 360;

    if(angle > 180)
        angle -= 360;

	return angle;
}
//Ausrichtung in Quaternionen
float Bno055::quaDataW(void)
{
	return (int16_t)(this->getWord(0x20)/(1<<14));
}
float Bno055::quaDataX(void)
{
	return (int16_t)(this->getWord(0x22)/(1<<14));
}
float Bno055::quaDataY(void)
{
	return (int16_t)(this->getWord(0x24)/(1<<14));
}
float Bno055::quaDataZ(void)
{
	return (int16_t)(this->getWord(0x26)/(1<<14));
}
//Lineare Beschleunigung Vektoren
float Bno055::liaDataX(void)
{
	return this->getWord(0x28)/100;
}
float Bno055::liaDataY(void)
{
	return this->getWord(0x2A)/100;
}
float Bno055::liaDataZ(void)
{
	return this->getWord(0x2C)/100;
}
//Gravitations Vektoren
float Bno055::grvDataX(void)
{
	return this->getWord(0x2E)/100;
}
float Bno055::grvDataY(void)
{
	return this->getWord(0x30)/100;
}
float Bno055::grvDataZ(void)
{
	return this->getWord(0x32)/100;
}
