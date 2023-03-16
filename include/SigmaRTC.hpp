#pragma once
#include <time.h>

typedef enum {
    RTC_AUTODETECT,
    RTC_DS3231,
    RTC_DS1307
} RTCType;
int ua_dst(const time_t* timer, int32_t* z);

class SigmaRTC {
public:
    SigmaRTC(RTCType type = RTC_AUTODETECT);
    virtual tm GetTime() = 0;
    virtual void SetTime(tm& t) = 0;
    void SetTime(time_t t, int tz = 2*ONE_HOUR);
    virtual ~SigmaRTC() {};
    RTCType type;
private:

 
};
