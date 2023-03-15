#include "SigmaDS3231.hpp"
#include <Wire.h>

SigmaDS3231::SigmaDS3231(byte addr) : address(addr) {
    Wire.begin();
}

tm SigmaDS3231::GetTime() {
    tm tm0;
    //Wire.begin();
    Wire.beginTransmission(address);
    Wire.write(0x0);
    byte res = Wire.endTransmission();
    if (res == 0) {
        Wire.requestFrom(address, (byte)7);
        tm0.tm_sec = unpackRegister(Wire.read());
        tm0.tm_min = unpackRegister(Wire.read());
        tm0.tm_hour = unpackHours(Wire.read());
        tm0.tm_wday = Wire.read();
        tm0.tm_mday = unpackRegister(Wire.read());
        byte m = Wire.read();
        tm0.tm_year = unpackRegister(Wire.read()) /*+ basicYear*/;
        if (m & 0x80) { // this is a century bit
            tm0.tm_year += 100;
            m &= 0x1F;
           
        }
        tm0.tm_mon = unpackRegister(m);
    }
    Wire.end();
    tm0.tm_isdst = -1;
    tm0.tm_yday = -1;
    return tm0;
}


byte SigmaDS3231::encodeHours(byte data) {
    if (data > 19)
        return ((0x2 << 4) | (data % 20));
    else if (data > 9)
        return ((0x1 << 4) | (data % 10));
    else
        return data;
}

byte SigmaDS3231::encodeMonth(byte data, int year) {
    if (year > 1999)
        return ((0x1 << 7) | encodeRegister(data));
    else
        return encodeRegister(data);
}

void SigmaDS3231::SetTime(tm& t) {
    Wire.beginTransmission(address);
    Wire.write(0x00);
    Wire.write(encodeRegister(t.tm_sec));
    Wire.write(encodeRegister(t.tm_min));
    Wire.write(encodeHours(t.tm_hour));
    Wire.write(t.tm_wday);
    Wire.write(encodeRegister(t.tm_mday));
    Wire.write(encodeMonth(t.tm_mon, t.tm_year));
    Wire.write(encodeRegister(t.tm_year));
    Wire.endTransmission();
}


byte SigmaDS3231::unpackRegister(byte data) {
    return ((data >> 4) * 10 + (data & 0xF));
}

byte SigmaDS3231::encodeRegister(byte data) {
    return (((data / 10) << 4) | (data % 10));
}

byte SigmaDS3231::unpackHours(byte data) {
    if (data & 0x20)
        return ((data & 0xF) + 20);
    else if (data & 0x10)
        return ((data & 0xF) + 10);
    else
        return (data & 0xF);
}