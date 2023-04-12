#ifdef ESP8266
#include <Esp.h>
#else
#include <Arduino.h>
#endif
#include <time.h>
#ifdef ESP8266
#define WIFI 1
#else
#define ETHERNET 1
#endif

#ifdef ETHERNET
#include <Ethernet.h>
#endif
#ifdef WIFI
#include <ESP8266WiFi.h>
//#include <ESP8266WiFiUdp.h>
#include <WiFiUdp.h>
#define WIFI_SSID "Sigma"
#define WIFI_PASS "kybwynyd"
#endif

#include "SigmaClock.hpp"

// DS1302 connection pins datPin, clkPin, cePin;
//#define DS3231 1
#define DS1302 1

#ifdef ESP8266
DS1302_Pins pin1302 = { D5,D6,D7 };
#else
DS1302_Pins pin1302 = { 14,15,16 };
#endif


void setup() { 
  Serial.begin(115200);
  //delay(1000);
  Serial.flush();
  while (!Serial) {
    delay(10);
  }
  Serial.println("-----------------------");
  Serial.println("Start!");
  
  tm t0, t1;
  char buf[100];
  
#ifdef ESP8266
  time_t tSystem = time(nullptr);
  localtime_r(&tSystem, &t0);
  Serial.print("System time:"); Serial.println(SigmaClock::PrintRaw(t0, buf));
#endif
  // Autodetect a type of connected RTC:
  // Prioritizes DS3231 over DS1302
  Serial.print("Type:"); Serial.println(SigmaClock::DetectRtcType());

#if DS3231  
  // Get time from DS3231 (by default)
  t0 = SigmaClock::GetClock();
  SigmaClock::PrintRaw(t0, buf);
  Serial.print("buf="); Serial.println(buf);
  Serial.print("DS3231:"); Serial.println(SigmaClock::PrintClock(t0));
  // set time in format seconds from 2000 year 2023-03-16 21:42:17 (and timezone)

  SigmaClock::SetClock(1678995732LU, 2 * ONE_HOUR);
  t0 = SigmaClock::GetClock();
  Serial.print("DS3231:"); Serial.println(SigmaClock::PrintClock(t0));
#endif
#if DS1302  
  // Get time from DS1302
  t0 = SigmaClock::GetClock(RTC_DS1302, pin1302);
  Serial.print("DS1302:"); //Serial.println(SigmaClock::PrintClock(t0));
  Serial.println(SigmaClock::PrintRaw(t0, buf));
  Serial.println(SigmaClock::PrintClock(t0));
  // set time in format seconds from 2000 year 2023-03-16 21:42:17 (and timezone)

 // SigmaClock::SetClock(1678995732LU, 2 * ONE_HOUR, RTC_DS1302, pin1302);
 // t0 = SigmaClock::GetClock(RTC_DS1302, pin1302);
 // Serial.print("DS1302:"); Serial.println(SigmaClock::PrintClock(t0));
#endif 
  
 /*  
  t0.tm_year = 05;
  t0.tm_mon = DECEMBER;
  t0.tm_mday = 31;
  t0.tm_hour = 23;
  t0.tm_min = 59;
  t0.tm_sec = 50;
  t0.tm_wday = WEDNESDAY;

  Serial.print("FormatI time: ");
  Serial.println(SigmaClock::PrintClock(t0));
  
#if DS3231
  SigmaClock::SetClock(t0);
  t1 = SigmaClock::GetClock();
  Serial.print("Get time from DS3231: ");
  Serial.println(SigmaClock::PrintClock(t1));
#endif
#if DS1302
  SigmaClock::SetClock(t0, RTC_DS1302, pin1302);
  t1 = SigmaClock::GetClock(RTC_DS1302, pin1302);
  Serial.print("Get time from DS1302: ");
  Serial.println(SigmaClock::PrintClock(t1));
#endif
  

  // Sync with NTP server
  
#ifdef ETHERNET
  Serial.println("Init Ethernet with DHCP");
  byte mac[6] = { 0x00, 0xAA, 0x22, 0x07, 0x69, 0x07 };

  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    } else if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    } else {
      Serial.println("Ethernet shield was found but DHCP failed to configure.  Try again.");
      //Ethernet.begin(mac, ip);
    }
  }
  EthernetLinkStatus link = Ethernet.linkStatus();
  if (EthernetLinkStatus::LinkOFF != link) {
    Serial.println("Ethernet has been initialized");
    //delay(1000);
  } else {
    Serial.println("Can't connect to Ethernet=");
  }
  Serial.print("DNS: ");
  Ethernet.dnsServerIP().printTo(Serial);
  Serial.println();
  Serial.print("My IP: ");
  Ethernet.localIP().printTo(Serial);
  Serial.println();
  
#else // WIFI
  Serial.println("Init WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");
  Serial.print("DNS: ");
  WiFi.dnsIP().printTo(Serial);
  Serial.println();
  Serial.print("My IP: ");
  WiFi.localIP().printTo(Serial);
  Serial.println();
#endif  

  Serial.println("Sync time");
  time_t t_sync;
#ifdef ESP8266
  tm tm1; 
  t_sync = SigmaClock::SyncClockSync(2 * ONE_HOUR, -1, &tm1);
  Serial.print("Sync time 8266: ");
  Serial.println(SigmaClock::PrintRaw(tm1, buf));
  t_sync = SigmaClock::SyncClockSync(2 * ONE_HOUR, -1, &tm1);
  Serial.print("Second time 8266: ");
  Serial.println(SigmaClock::PrintRaw(tm1, buf));
#else
  t_sync = SigmaClock::SyncClock();
#endif
#ifdef DS3231
  SigmaClock::SetClock(t_sync, 2 * ONE_HOUR);
  t1 = SigmaClock::GetClock();
  Serial.print("Sync time DS3231: ");
#endif
  
#ifdef DS1302
  SigmaClock::SetClock(t_sync, 2 * ONE_HOUR, RTC_DS1302, pin1302); 
  t1 = SigmaClock::GetClock(RTC_DS1302, pin1302);
  Serial.print("Sync time DS1302: ");
#endif
  Serial.println(SigmaClock::PrintClock(t1));
  Serial.print("Raw time: ");
  Serial.println(SigmaClock::PrintRaw(t1, buf));
*/
  Serial.println("Done");
}

void loop() {
  
}