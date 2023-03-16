#include "SigmaRTC.hpp"

int ua_dst(const time_t* timer, int32_t* z) {
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



SigmaRTC::SigmaRTC(RTCType type) {
    this->type = type;
}

void SigmaRTC::SetTime(time_t t, int tz) {
    tm tm0;
    set_zone(tz);
    set_dst(ua_dst);
    localtime_r(&t, &tm0);
    SetTime(tm0);
}


byte SigmaRTC::decodeRegister(byte data) {
    return ((data >> 4) * 10 + (data & 0xF));
}

byte SigmaRTC::encodeRegister(byte data) {
    return (((data / 10) << 4) | (data % 10));
}
