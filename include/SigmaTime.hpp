#pragma once

#ifdef ESP8266
typedef enum {
    SUNDAY = 0,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY
} _WEEK_DAYS_;

typedef enum {
    JANUARY = 0,
    FEBRUARY,
    MARCH,
    APRIL,
    MAY,
    JUNE,
    JULY,
    AUGUST,
    SEPTEMBER,
    OCTOBER,
    NOVEMBER,
    DECEMBER
} _MONTHS_;

#define ONE_HOUR 3600
#define UNIX_OFFSET 946684800
#define NTP_OFFSET 3155673600

#define LEAP_YEAR(Y)      ( (Y)>0) && !((Y)%4) && ( ((Y)%100) || !((Y)%400) )

int month_length(int year, int month /* 1-12 */);
#endif
