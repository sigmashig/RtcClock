#pragma once
#include <time.h>
#include "SigmaRTC.hpp"
#include "Ethernet.h"

class RTCClock {
public:
    typedef enum {
        CAL_SERVER_WORLDTIMEAPI,
        CAL_SERVER_NTP
    } CalendarServerType;


    static _WEEK_DAYS_ DayYesterday(_WEEK_DAYS_ day);

    static time_t SyncClock(CalendarServerType type = CAL_SERVER_NTP);
    static bool IsTimestampValid(time_t t);
    static bool IsTimestampValid(tm t);
    static tm GetTime(RTCType rtcType = RTC_DS3231, DS1302_Pins pins= { 0,0,0 });
    static void SetTime(tm& t, RTCType rtcType = RTC_DS3231, DS1302_Pins pins = { 0,0,0 });
    static void SetTime(time_t t, int tz = 2 * ONE_HOUR, RTCType rtcType = RTC_DS3231, DS1302_Pins pins = { 0,0,0 });
    static char* PrintRaw(tm& t, char* buf);
    static char* PrintClock(tm& t);
    
private:
    static time_t getNtpTime();
    static time_t readWorldTimeApi();
    static time_t worldTimeApiParseResponse(EthernetClient* client);
    static bool extractBody(EthernetClient* client, char* body);
    static time_t worldTimeApiParseJson(const char* buf);
    static bool httpConnection(EthernetClient* client, const char* url, const char* path, unsigned long port);
    static SigmaRTC* getRtc(RTCType rtcType, DS1302_Pins pins);
};

