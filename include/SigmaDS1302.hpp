#pragma once
#ifdef ESP8266
#include <Esp.h>
#else
#include <Arduino.h>
#endif
#include "SigmaRTC.hpp"

class SigmaDS1302: public SigmaRTC {
public:
    SigmaDS1302(DS1302_Pins& pins);
    tm GetTime();
    void SetTime(tm& t);
    bool IsConnected(void){ return true; }; // there is no possibility to check if the RTC is connected
    ~SigmaDS1302();
    bool IsHalted();

private:
    const byte cePin;
    const byte datPin;
    const byte sclkPin;
    int datDirection;

    //void writeOut(byte value, bool readAfter=false);
    byte readIn();
    void writeOut(byte value);
    void nextBit();
    void setDirection(int direction);
    //byte readRegister(const byte reg);
    //void writeRegister(byte reg, byte value);
    //void writeProtect(const bool enable);
    //void halt(const bool enable);
    byte bcdToDec(const byte bcd);
    byte decToBcd(const byte dec);
    byte hourFromRegisterValue(const byte value);
    bool isValid();

    void init();
    void halt();
    void prepareRead(byte address);
    void prepareWrite(byte address);

    void end();

};