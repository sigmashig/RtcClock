#pragma once
#ifdef ESP8266
#include <Esp.h>
#else
#include <Arduino.h>
#endif

#ifdef ESP8266
#define WIFI 2
#include <WiFiClient.h>
#include <WiFiUdp.h>
#else
#define ETHERNET 2
#include <EthernetUdp.h>
#endif
#include <time.h>
#include "SigmaTime.hpp"
#include "SigmaRTC.hpp"
#include <Client.h>

class SigmaClock {
public:
    typedef enum {
        CAL_SERVER_WORLDTIMEAPI,
        CAL_SERVER_NTP
    } CalendarServerType;

 
    static _WEEK_DAYS_ DayYesterday(_WEEK_DAYS_ day);

    static time_t SyncClock(CalendarServerType type = CAL_SERVER_NTP);
    static bool IsTimestampValid(time_t t);
    static bool IsTimestampValid(tm t);
    static RTCType DetectRtcType();
    static tm GetClock(RTCType rtcType = RTC_AUTODETECT, DS1302_Pins pins= { 0,0,0 });
    static void SetClock(tm& t, RTCType rtcType = RTC_AUTODETECT, DS1302_Pins pins = { 0,0,0 });
    static void SetClock(time_t t, int tz = 2 * ONE_HOUR, RTCType rtcType = RTC_AUTODETECT, DS1302_Pins pins = { 0,0,0 });
    static char* PrintRaw(tm& t, char* buf);
    static char* PrintClock(tm& t);
#ifdef ESP8266
    static void SyncClockAsync(int tz, int dst = -1, tm* lTime = nullptr);
    static time_t SyncClockSync(int tz, int dst = -1, tm* lTime = nullptr);
#endif
    static int GetDstUA(const time_t* timer, int32_t* unused = nullptr);
private:
   
    static time_t getNtpTime();
    static time_t readWorldTimeApi();
    static time_t worldTimeApiParseResponse(Client* client);
    static bool extractBody(Client* client, char* body);
    static time_t worldTimeApiParseJson(const char* buf);
    static bool httpConnection(Client* client, const char* url, const char* path, unsigned long port);
    static SigmaRTC* getRtc(RTCType rtcType, DS1302_Pins pins);
};

