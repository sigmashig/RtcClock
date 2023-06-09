#pragma once
#ifdef ESP8266
#include <Esp.h>
#else
#include <Arduino.h>
#endif
#include <time.h>
#include "SigmaTime.hpp"


typedef enum {
    RTC_AUTODETECT,
    RTC_DS3231,
    RTC_DS1302
} RTCType;

typedef struct {
    byte datPin, clkPin, cePin;
} DS1302_Pins;

int ua_dst(const time_t* timer, int32_t* z);

class SigmaRTC {
public:
    SigmaRTC(RTCType type = RTC_AUTODETECT);
    virtual tm GetTime() = 0;
    virtual void SetTime(tm& t) = 0;
    void SetTime(time_t t, int tz = 2*ONE_HOUR);
    virtual ~SigmaRTC() {};
    virtual bool IsConnected() = 0;
    RTCType type;
protected:
    byte decodeRegister(byte data);
    byte encodeRegister(byte data);

private:

 
};
