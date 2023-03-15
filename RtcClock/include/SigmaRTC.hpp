#pragma once
#include <time.h>

typedef enum {
    RTC_AUTODETECT,
    RTC_DS3231,
    RTC_DS1307
} RTCType;

class SigmaRTC {
public:
    SigmaRTC(RTCType type = RTC_AUTODETECT);
    virtual tm GetTime() = 0;
    virtual void SetTime(tm& t) = 0;
    ~SigmaRTC() {};
    RTCType type;
private:

    virtual void prevent_instantiation() = 0;

};
