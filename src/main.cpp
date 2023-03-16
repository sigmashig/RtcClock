#include <Arduino.h>
#include <time.h>
#include "SigmaClock.hpp"

DS1302_Pins pin1302 = { 14,15,16 };
void setup() {
  Serial.begin(115200);

  
  // put your setup code here, to run once:
  tm t0, t1;
  char buf[100];
  //RTCClock rClock = RTCClock();
  //t = rClock.GetTime();

  Serial.print("Type:"); Serial.println(SigmaClock::DetectRtcType());

  t0 = SigmaClock::GetClock();
  Serial.print("DS3231:"); Serial.println(SigmaClock::PrintClock(t0));
  t0 = SigmaClock::GetClock(RTC_DS1302, pin1302);
  Serial.print("DS1302:"); Serial.println(SigmaClock::PrintClock(t0));
  

  SigmaClock::SetClock(1678995732LU, 2 * ONE_HOUR, RTC_DS1302, pin1302);
  SigmaClock::SetClock(1678995732LU, 2 * ONE_HOUR, RTC_DS3231, pin1302);

  t0 = SigmaClock::GetClock();
  Serial.print("DS3231:"); Serial.println(SigmaClock::PrintClock(t0));
  t0 = SigmaClock::GetClock(RTC_DS1302, pin1302);
  Serial.print("DS1302:"); Serial.println(SigmaClock::PrintClock(t0));

  
  
  /*
  t0.tm_year = 99;
  t0.tm_mon = DECEMBER;
  t0.tm_mday = 31;
  t0.tm_hour = 23;
  t0.tm_min = 59;
  t0.tm_sec = 50;
  t0.tm_wday = 3;

  RTCClock::PrintRaw(t0, buf);
  Serial.print("Initial time: ");
  Serial.println(buf);
  Serial.print("FormatI time: ");
  Serial.println(RTCClock::PrintClock(t0));
  
  
    RTCClock::SetTime(t0, RTC_DS1302, pin1302);
    t1 = RTCClock::GetTime(RTC_DS1302, pin1302);
    Serial.print("Get time: ");
    RTCClock::PrintRaw(t1, buf);
    Serial.println(buf);
    Serial.println(RTCClock::PrintClock(t1));
    RTCClock::SetTime(1678995732LU, 2 * ONE_HOUR, RTC_DS1302, pin1302);
    t1 = RTCClock::GetTime(RTC_DS1302, pin1302);
    Serial.print("2 Get time: ");
    RTCClock::PrintRaw(t1, buf);
    Serial.println(buf);
    Serial.println(RTCClock::PrintClock(t1));

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
  t_sync = RTCClock::SyncClock();
  RTCClock::SetTime(t_sync);
  t1 = RTCClock::GetTime();
  Serial.print("Sync time: ");
  Serial.println(RTCClock::PrintClock(t1));
*/
}

void loop() {
  /*
  // put your main code here, to run repeatedly:
  char buf[100];
  tm t1;
  t1 = RTCClock::GetTime(RTC_DS1302, pin1302);
  Serial.print("Get time: ");
 // RTCClock::PrintRaw(t1, buf);
 // Serial.println(buf);
  Serial.println(RTCClock::PrintClock(t1));
  delay(3500);
*/
}