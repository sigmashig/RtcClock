#include <Arduino.h>
#include <time.h>
#include "SigmaClock.hpp"

// DS1302 connection pins datPin, clkPin, cePin;
DS1302_Pins pin1302 = { 14,15,16 };
void setup() {
  Serial.begin(115200);

  tm t0, t1;
  char buf[100];

  // Autodetect a type of connected RTC:
  // Prioritizes DS3231 over DS1302
  Serial.print("Type:"); Serial.println(SigmaClock::DetectRtcType());

  
  // Get time from DS3231 (by default)
  t0 = SigmaClock::GetClock();
  Serial.print("DS3231:"); Serial.println(SigmaClock::PrintClock(t0));
  // Get time from DS1302
  t0 = SigmaClock::GetClock(RTC_DS1302, pin1302);
  Serial.print("DS1302:"); Serial.println(SigmaClock::PrintClock(t0));
  
  // set time in format seconds from 2000 year 2023-03-16 21:42:17 (and timezone)
  
  SigmaClock::SetClock(1678995732LU, 2 * ONE_HOUR, RTC_DS1302, pin1302);
  SigmaClock::SetClock(1678995732LU, 2 * ONE_HOUR, RTC_DS3231, pin1302);

  t0 = SigmaClock::GetClock();
  Serial.print("DS3231:"); Serial.println(SigmaClock::PrintClock(t0));
  t0 = SigmaClock::GetClock(RTC_DS1302, pin1302);
  Serial.print("DS1302:"); Serial.println(SigmaClock::PrintClock(t0));

  
  
  
  t0.tm_year = 99;
  t0.tm_mon = DECEMBER;
  t0.tm_mday = 31;
  t0.tm_hour = 23;
  t0.tm_min = 59;
  t0.tm_sec = 50;
  t0.tm_wday = WEDNESDAY;

  Serial.print("FormatI time: ");
  Serial.println(SigmaClock::PrintClock(t0));
  
  
  SigmaClock::SetClock(t0, RTC_DS1302, pin1302);
  t1 = SigmaClock::GetClock(RTC_DS1302, pin1302);
  Serial.print("Get time from DS1302: ");
    Serial.println(SigmaClock::PrintClock(t1));

    SigmaClock::SetClock(t0);
    t1 = SigmaClock::GetClock();
    Serial.print("Get time from DS3231: ");
    Serial.println(SigmaClock::PrintClock(t1));

    // Sync with NTP server

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
  Ethernet.localIP().printTo(Serial);
  Serial.println();


  time_t t_sync;
  t_sync = SigmaClock::SyncClock();
  SigmaClock::SetClock(t_sync, 2 * ONE_HOUR);
  SigmaClock::SetClock(t_sync, 2 * ONE_HOUR, RTC_DS1302, pin1302);
  t1 = SigmaClock::GetClock();
  Serial.print("Sync time DS3231: ");
  Serial.println(SigmaClock::PrintClock(t1));

  t1 = SigmaClock::GetClock(RTC_DS1302, pin1302);
  Serial.print("Sync time DS1302: ");
  Serial.println(SigmaClock::PrintClock(t1));

}

void loop() {
  
}