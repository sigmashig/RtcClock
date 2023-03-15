#pragma once
#include <time.h>
#include "SigmaRTC.hpp"

class RTCClock {
public:
    static tm GetTime(RTCType rtcType = RTC_DS3231);
    static void SetTime(tm& t, RTCType rtcType = RTC_DS3231);
    static char* Print0(tm& t);
private:
};
