#include <time.h>
#include "SigmaTime.hpp"

int month_length(int year, int month /* 1-12 */) {
    int days = 31;
    if (month == 4 || month == 6 || month == 9 || month == 11) {
        days = 30;
    } else if (month == 2) {
        days = 28;
        if (LEAP_YEAR(year) != 0) {
            days = 29;
        }
    }
    return days;
}
