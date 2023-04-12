#include "SigmaClock.hpp"
#include "SigmaRTC.hpp"
#include "SigmaDS3231.hpp"
#include "SigmaDS1302.hpp"
#include <ArduinoJson.h>
//#include <Udp.h>
//#include <EthernetUdp.h>
//#include <WiFiUdp.h>

tm SigmaClock::GetClock(RTCType rtcType, DS1302_Pins pins) {
    tm tm0;
    tm0.tm_year = 123;

    SigmaRTC* rtc = getRtc(rtcType, pins);
    tm0 = rtc->GetTime();
    delete rtc;
    return tm0;
}

void SigmaClock::SetClock(tm& t, RTCType rtcType, DS1302_Pins pins) {

    SigmaRTC* rtc = getRtc(rtcType, pins);
    rtc->SetTime(t);
    delete rtc;
}


void SigmaClock::SetClock(time_t t, int tz, RTCType rtcType, DS1302_Pins pins) {
    SigmaRTC* rtc = getRtc(rtcType, pins);
    rtc->SetTime(t, tz);
    delete rtc;
}


char* SigmaClock::PrintRaw(tm& t, char* buf) {
    sprintf(buf, "%d/%d/%d %d:%d:%d wday=%d yday=%d dst=%d", t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, t.tm_wday, t.tm_yday, t.tm_isdst);
    return buf;
}

char* SigmaClock::PrintClock(tm& t) {
    static char strClock[50];
    if (!IsTimestampValid(t)) {
        sprintf(strClock,"Invalid timestamp");
    } else {
        strftime(strClock, sizeof(strClock), "%Y-%m-%d %H:%M:%S %A (%w) [%b %B]", &t);
    }
    return strClock;
}

_WEEK_DAYS_ SigmaClock::DayYesterday(_WEEK_DAYS_ day) {
    if (day == MONDAY) {
        day = SUNDAY;
    } else {
        day = (_WEEK_DAYS_)(day - 1);
    }
    return day;
}


time_t SigmaClock::getNtpTime() {
    
#ifdef ETHERNET
    Ethernet Udp;
#else
    WiFiUDP Udp;
#endif
    unsigned int localPort = 8888;       // local port to listen for UDP packets
    char timeServer[] = "pool.ntp.org"; // NTP server
    int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
    byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

    unsigned long seconds = 0;
 
    Udp.begin(localPort);
 
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;

    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
 
    Udp.beginPacket(timeServer, 123); // NTP requests are to port 123
    Udp.write(packetBuffer, NTP_PACKET_SIZE);
    Udp.endPacket();
 
    int nTry = 20;
    int ret = 0;
    while (ret != 48 && nTry > 0) {
        delay(100);
        ret = Udp.parsePacket();
        nTry--;
    }
    if (ret == 48) {
        // We've received a packet, read the data from it
        Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

        // the timestamp starts at byte 40 of the received packet and is four bytes,
        // or two words, long. First, extract the two words:

        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
        // combine the four bytes (two words) into a long integer
         // this is NTP time (seconds since Jan 1 1900):
        seconds = highWord << 16 | lowWord;
        
        //seconds -= 2208988800UL; // subtract seventy years: 1970-1900 NTP_OFFSET
        seconds += -NTP_OFFSET + UNIX_OFFSET;
    }

    Udp.stop();
    return seconds;
}

#ifdef ESP8266
void SigmaClock::SyncClockAsync(int tz, int dst, tm* lTime) {

    if (dst < 0) {
        time_t tCurrent = time(nullptr);
        dst = GetDstUA(&tCurrent);
    }
    
    configTime(tz, dst, "pool.ntp.org", "time.nist.gov");
}

time_t SigmaClock::SyncClockSync(int tz, int dst, tm* lTime) {

    SyncClockAsync(tz, dst, lTime);
    time_t t = time(nullptr);
    while (t < 8 * 3600 * 2) {
        delay(500);
        t = time(nullptr);
    }
    if (lTime != nullptr) {
        localtime_r(&t, lTime);
        //lTime->tm_isdst = dst;
    }
    return t;
}

#endif

time_t SigmaClock::SyncClock(CalendarServerType type) {
    time_t t = 0;
    switch (type) {
    case CAL_SERVER_WORLDTIMEAPI:
        t = readWorldTimeApi();
        break;
    case CAL_SERVER_NTP:
        t = getNtpTime();
        break;
    }
    return t;
}

bool SigmaClock::IsTimestampValid(time_t t) {
    // TODO: check if timestamp is valid
    return t > 0;
}

bool SigmaClock::IsTimestampValid(tm t) {

    if (t.tm_year < 2023 || t.tm_year >= 2100) {
        return false;
    }
    if (t.tm_mon < 0 || t.tm_mon > 12) {
        return false;
    }
    if (t.tm_mday < 0 || t.tm_mday > month_length(t.tm_year + 1900, t.tm_mon + 1)) {
        return false;
    }
    if (t.tm_hour < 0 || t.tm_hour > 23) {
        return false;
    }
    if (t.tm_min < 0 || t.tm_min > 59) {
        return false;
    }
    if (t.tm_sec < 0 || t.tm_sec > 59) {
        return false;
    }
    return t.tm_year > 0;
}


time_t SigmaClock::readWorldTimeApi() {
#ifdef ETHERNET
    EthernetClient* client = new EthernetClient();
#else
    WiFiClient* client = new WiFiClient();
#endif
    const char* server = "worldtimeapi.org";
    char* path = (char*)"/api/timezone/GMT";
    time_t t = 0;

    if (httpConnection(client, server, path, 80)) {
        t = worldTimeApiParseResponse(client);
    }
    client->stop();
    delete client;
    return t;
}

time_t SigmaClock::worldTimeApiParseResponse(Client* client) {
    char buf[512];
    time_t t = 0;

    if (extractBody(client, buf)) {
        t = worldTimeApiParseJson(buf);
    }
    return t;
}

bool SigmaClock::extractBody(Client* client, char* body) {
    bool res = false;
    String s;

    if (client->available()) {
        s = client->readStringUntil('\n');
        if (s.indexOf("200 OK") != -1) {
            while (client->available()) {
                s = client->readStringUntil('\n');
            }
            if (s.length() != 0) {
                strcpy(body, s.c_str());
                res = true;
            }
        }
    }
    return res;
}



time_t SigmaClock::worldTimeApiParseJson(const char* buf) {
    time_t t = 0;

    if (buf[0] != 0) {
        //const size_t CAPACITY = JSON_OBJECT_SIZE(25);
        StaticJsonDocument<400> doc;
        //DynamicJsonDocument doc(200);
        DeserializationError error = deserializeJson(doc, buf);
        if (!error) {

            // extract the data
            JsonObject root = doc.as<JsonObject>();
            if (root.containsKey("unixtime")) {
                t = root["unixtime"];
            }
            /*
            if (root.containsKey("datetime")) {

                String s = root["datetime"]; //"2022-01-09T15:32:39.409582+02:00"
                tm0.tm_year = s.substring(0, 4).toInt() - 1900;
                tm0.tm_mon = s.substring(5, 7).toInt() - 1;
                tm0.tm_mday = s.substring(8, 10).toInt() - 1;
                tm0.tm_hour = s.substring(11, 13).toInt();
                tm0.tm_min = s.substring(14, 16).toInt();
                tm0.tm_min = s.substring(17, 19).toInt();

                //  res = true;
            }
            else if (root.containsKey("day_of_week")) {
            //    tm.Wday = root["day_of_week"];
            //    tm.Wday += 1; //Sunday is 1
            //}
*/
        }
    }
    return t;
}



bool SigmaClock::httpConnection(Client* client, const char* url, const char* path, unsigned long port) {

    int numbTry = 0;
    int len = 0;
    bool res = false;
    unsigned int connRes;

    while (numbTry < 3 && len == 0) {
        numbTry++;

        connRes = client->connect(url, port);
        if (connRes) {
            // Make a HTTP request:
            char buf[100];
            sprintf(buf, "GET %s HTTP/1.1", path);
            client->println(buf);
            sprintf(buf, "Host: %s", url);
            client->println(buf);
            client->println("Connection: close");
            client->println();

            int nTry = 0;
            len = client->available();
            while (nTry <= 5 && len == 0) {
                delay(2000);
                nTry++;
                len = client->available();
            }

        } else {
            break;
        }
    }
    if (len != 0) {
        res = true;
    }
    return res;
}

SigmaRTC* SigmaClock::getRtc(RTCType rtcType, DS1302_Pins pins) {
    switch (rtcType) {
    case RTC_DS3231:
        return new SigmaDS3231();
        break;
    case RTC_DS1302:
        return new SigmaDS1302(pins);
        break;
    case RTC_AUTODETECT:
        if (pins.cePin == 0 || pins.datPin == 0 || pins.clkPin == 0) {
            return new SigmaDS3231();
        } else {
            SigmaRTC* rtc = new SigmaDS3231();
            if (rtc->IsConnected()) {
                return rtc;
            }
            return new SigmaDS1302(pins);
        }
        break;
    }
    return nullptr;
}

RTCType SigmaClock::DetectRtcType() {
    SigmaDS3231 rtc1;
    if (rtc1.IsConnected()) {
        return RTC_DS3231;
    }
    return RTC_DS1302;
}

int SigmaClock::GetDstUA(const time_t* timer, int32_t* unused) {
    struct tm       tmptr;
    uint8_t         month, mday, hour, day_of_week, d;
    int             n;

    /* obtain the variables */
    gmtime_r(timer, &tmptr);
    month = tmptr.tm_mon;
    day_of_week = tmptr.tm_wday;
    mday = tmptr.tm_mday - 1;
    hour = tmptr.tm_hour;

    if ((month > MARCH) && (month < OCTOBER))
        return ONE_HOUR;

    if (month < MARCH)
        return 0;
    if (month > OCTOBER)
        return 0;

    /* determine mday of last Sunday */
    n = tmptr.tm_mday - 1;
    n -= day_of_week;
    n += 7;
    d = n % 7;  /* date of first Sunday */

    n = 31 - d;
    n /= 7; /* number of Sundays left in the month */

    d = d + 7 * n;  /* mday of final Sunday */
    if (month == MARCH) {
        if (d < mday) {
            return ONE_HOUR;
        } else if (d > mday) {
            return 0;
        } else if (hour < 2) {
            return 0;
        }
        return ONE_HOUR;
    } else if (d < mday) {
        return 0;
    } else if (d > mday) {
        return ONE_HOUR;
    } else if (hour < 2) {
        return ONE_HOUR;
    }
    return 0;
}
