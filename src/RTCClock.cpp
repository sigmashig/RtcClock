#include "RTCClock.hpp"
#include "SigmaRTC.hpp"
#include "SigmaDS3231.hpp"

tm RTCClock::GetTime(RTCType rtcType) {
    tm tm0;
    tm0.tm_year = 123;
    
    SigmaRTC *rtc = new SigmaDS3231();
    tm0 = rtc->GetTime();
    //delete (SigmaDS3231*)rtc;
    return tm0;
}

void RTCClock::SetTime(tm& t, RTCType rtcType) {
    SigmaRTC *rtc = new SigmaDS3231();
    rtc->SetTime(t);
    //delete rtc;
}

char* RTCClock::Print0(tm& t) {
    static char buf[100];
    sprintf(buf, "%d/%d/%d %d:%d:%d %d %d %d", t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, t.tm_wday, t.tm_yday, t.tm_isdst);
    return buf;
}
    