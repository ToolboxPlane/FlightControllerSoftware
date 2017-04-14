#include "SBus.hpp"
#include "mbed.h"

SBus::SBus(PinName rx, PinName tx) : serial(tx, rx){
    serial.baud(100000);
    serial.format(8, Serial::Even, 2);

    serial.attach(this, &SBus::rxCallback);

    rxMux = 0;
    badConnCount = 0;
}

void SBus::rxCallback(){
    uint8_t data = serial.getc();

    if(rxMux == 0){
        if(data == SBUS_START_BYTE)
            rxMux = 1;

    }else if(rxMux == 24){
        if(data == SBUS_END_BYTE){
            status = rxData[22];

            if(status == 0){
                for(uint8_t c=0; c<16; c++){
                    uint8_t startIndex = (c*11)/8;
                    uint8_t startBit = (c*11)%8;

                    uint16_t currWord = 0;
                    for(uint8_t d=0; d<11; d++){
                        currWord |= ((rxData[startIndex] & 0b1<<startBit)?1:0) << d;

                        if(++startBit >= 8){
                            startBit = 0;
                            startIndex++;
                        }
                    }
                    channels[c] = currWord;

                    badConnCount = 0;
                }
            } else {
                if(++badConnCount >= 10){
                    badConnCount = 10;
                }
            }

            rxMux = 0;
        }else{
            rxMux = 0;
        }
    }else{
        rxData[rxMux-1] = data;
        rxMux++;
    }
}

uint16_t SBus::getChannel(uint8_t channel){
    if(channel <= 15)
        return channels[channel];
    else
        return 0;
}

uint8_t SBus::getStatusByte(){
    return status;
}

uint8_t SBus::frameLost(){
    return (status & (0b1<<3))?1:0;
}

uint8_t SBus::failSave(){
    return (status & (0b1<<2))?1:0;
}

uint8_t SBus::badConnection(){
    return badConnCount >= 10;
}

uint8_t SBus::getDigital(uint8_t channel){
    return (status & (0b1<<channel))?1:0;
}
