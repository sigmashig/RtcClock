#include "RTCClock.hpp"
#include "SigmaRTC.hpp"
#include "SigmaDS3231.hpp"
#include "SigmaDS1302.hpp"
#include <ArduinoJson.h>


tm RTCClock::GetClock(RTCType rtcType, DS1302_Pins pins) {
    tm tm0;
    tm0.tm_year = 123;
    
    SigmaRTC *rtc = getRtc(rtcType, pins);
    tm0 = rtc->GetTime();
    delete rtc;
    return tm0;
}

void RTCClock::SetClock(tm& t, RTCType rtcType, DS1302_Pins pins) {
   
    SigmaRTC* rtc = getRtc(rtcType, pins);
    rtc->SetTime(t);
    delete rtc;
}


void RTCClock::SetClock(time_t t, int tz, RTCType rtcType, DS1302_Pins pins) {
    SigmaRTC* rtc = getRtc(rtcType, pins);
    rtc->SetTime(t, tz);
    delete rtc;
}


char* RTCClock::PrintRaw(tm& t, char* buf) {
    sprintf(buf, "%d/%d/%d %d:%d:%d %d %d %d", t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, t.tm_wday, t.tm_yday, t.tm_isdst);
    return buf;
}

char* RTCClock::PrintClock(tm& t) {
    static char strClock[50];
    strftime(strClock, sizeof(strClock), "%Y-%m-%d %H:%M:%S %A (%w) [%b %B]", &t);
    return strClock;
}

_WEEK_DAYS_ RTCClock::DayYesterday(_WEEK_DAYS_ day) {
    if (day == MONDAY) {
        day = SUNDAY;
    } else {
        day = (_WEEK_DAYS_)(day - 1);
    }
    return day;
}


time_t RTCClock::getNtpTime() {
    EthernetUDP Udp;
    unsigned int localPort = 8888;       // local port to listen for UDP packets
    char timeServer[] = "pool.ntp.org"; // NTP server
    int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
    byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

    unsigned long seconds = 0;
    Serial.println("Starting connection to server...");
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
    Serial.println("Waiting for response...");
    int nTry = 20;
    int ret = 0;
    while (ret != 48 && nTry > 0) {
        delay(100);
        ret = Udp.parsePacket();
        Serial.println(".");
        nTry--;
    }
    Serial.print("Response received: ");Serial.println(ret);
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
        seconds -= NTP_OFFSET;
    }
    Udp.stop();
    Serial.print("Seconds since Jan 1 1970 = ");Serial.println(seconds);
    return seconds;
}

time_t RTCClock::SyncClock(CalendarServerType type) {
    time_t t = 0;
    if (Ethernet.linkStatus() != LinkOFF) {
        switch (type) {
        case CAL_SERVER_WORLDTIMEAPI:
            t = readWorldTimeApi();
            break;
        case CAL_SERVER_NTP:
            t = getNtpTime();
            break;
        }
    }
    return t;
}

bool RTCClock::IsTimestampValid(time_t t) {
    // TODO: check if timestamp is valid
    return t > 0;
}

bool RTCClock::IsTimestampValid(tm t) {
    
    if (t.tm_year < 2023 || t.tm_year >= 2100) {
        return false;
    }
    if (t.tm_mon < 0 || t.tm_mon > 12) {
        return false;
    }
    if (t.tm_mday < 0 || t.tm_mday > month_length(t.tm_year +1900 , t.tm_mon +1)) {
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


time_t RTCClock::readWorldTimeApi() {
    EthernetClient* client = new EthernetClient();
    const char* server = "worldtimeapi.org";
    char* path = (char*)"/api/timezone/GMT";
    time_t t=0;

    if (httpConnection(client, server, path, 80)) {
        t = worldTimeApiParseResponse(client);
    }
    client->stop();
    delete client;
    return t;
}

time_t RTCClock::worldTimeApiParseResponse(EthernetClient* client) {
    char buf[512];
    time_t t=0;

    if (extractBody(client, buf)) {
        t = worldTimeApiParseJson(buf);
    }
    return t;
}

bool RTCClock::extractBody(EthernetClient* client, char* body) {
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



time_t RTCClock::worldTimeApiParseJson(const char* buf) {
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



bool RTCClock::httpConnection(EthernetClient* client, const char* url, const char* path, unsigned long port) {

    int numbTry = 0;
    int len = 0;
    bool res = false;
    unsigned int connRes;

    Serial.println("Connecting to server");
    while (numbTry < 3 && len == 0) {
        numbTry++;

        Serial.println(numbTry);
        Serial.println(url);
        connRes = client->connect(url, port);
        if (connRes) {
            // Make a HTTP request:
            Serial.println("Connected to server");
            char buf[100];
            sprintf(buf, "GET %s HTTP/1.1", path);
            client->println(buf);
            sprintf(buf, "Host: %s", url);
            client->println(buf);
            client->println("Connection: close");
            client->println();

            int nTry = 0;
            len = client->available();
            Serial.println(len);
            while (nTry <= 5 && len == 0) {
                delay(2000);
                nTry++;
                len = client->available();
            }
            Serial.print("len=");
            Serial.println(len);

        } else {
            Serial.println("Connection failed");
            break;
        }
    }
    Serial.println("Connection done");
    if (len != 0) {
        res = true;
    }
    return res;
}

SigmaRTC* RTCClock::getRtc(RTCType rtcType, DS1302_Pins pins) {
    switch (rtcType)
    {
    case RTC_DS3231:
        return new SigmaDS3231();
        break;
    case RTC_DS1302:
        return new SigmaDS1302(pins);
        break;
    case RTC_AUTODETECT:
        if (pins.cePin==0 || pins.datPin==0 || pins.clkPin==0) {
            return new SigmaDS3231();
        }
        else {
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

RTCType RTCClock::DetectRtcType() {
    SigmaDS3231 rtc1;
    if (rtc1.IsConnected()) {
        return RTC_DS3231;
    }
    return RTC_DS1302;
}

