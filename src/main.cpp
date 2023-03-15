#include <Arduino.h>
#include <time.h>
#include "RTCClock.hpp"

void setup() {
  Serial.begin(115200);

  
  // put your setup code here, to run once:
  tm t0, t1;
  char buf[100];
  //RTCClock rClock = RTCClock();
  //t = rClock.GetTime();

  t0.tm_year = 99;
  t0.tm_mon = 12;
  t0.tm_mday = 31;
  t0.tm_hour = 23;
  t0.tm_min = 59;
  t0.tm_sec = 55;
  t0.tm_wday = 8;

  strcpy(buf, RTCClock::Print0(t0));
  Serial.print("Initial time: ");
  Serial.println(buf);
  RTCClock::SetTime(t0);
  
  t1 = RTCClock::GetTime();
  //strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S %w %z %a %A", &t);
  strcpy(buf, RTCClock::Print0(t1));
  Serial.print("Get time: ");
  Serial.println(buf);
}

void loop() {
  // put your main code here, to run repeatedly:
  char buf[100];
  tm t1;
  
  t1 = RTCClock::GetTime();
  //strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S %w %z %a %A", &t);
  strcpy(buf, RTCClock::Print0(t1));
  Serial.print("Get time: ");
  Serial.println(buf);
  delay(1500);
}