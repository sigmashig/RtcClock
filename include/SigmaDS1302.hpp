#pragma once

#include <Arduino.h>
#include "SigmaRTC.hpp"

class SigmaDS1302: public SigmaRTC {
public:
    SigmaDS1302(DS1302_Pins& pins);
    tm GetTime();
    void SetTime(tm& t);
    bool IsConnected(void){ return true; }; // there is no possibility to check if the RTC is connected
    ~SigmaDS1302();
private:
    const byte cePin;
    const byte ioPin;
    const byte sclkPin;


    void writeOut(byte value, bool readAfter=false);
    byte readIn();
    byte readRegister(const byte reg);
    void writeRegister(byte reg, byte value);
    void writeProtect(const bool enable);
    void halt(const bool enable);
    byte bcdToDec(const byte bcd);
    byte decToBcd(const byte dec);
    byte hourFromRegisterValue(const byte value);

};