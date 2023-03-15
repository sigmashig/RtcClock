#pragma once
#include <Arduino.h>
#include "SigmaRTC.hpp"

class SigmaDS3231 : public SigmaRTC{
public:
    SigmaDS3231(byte addr = 0x68);
    tm GetTime();
    void SetTime(tm& t);
    ~SigmaDS3231() {};
private:
    const byte address;
    const unsigned int basicYear = 1970;
    
    byte unpackRegister(byte data);
    byte encodeRegister(byte data);
    byte encodeHours(byte data);
    byte encodeMonth(byte data, int year);
    byte unpackHours(byte data);

    void prevent_instantiation() {};
};

