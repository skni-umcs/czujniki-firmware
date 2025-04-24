#ifndef TIMECONSTANTS_H
#define TIMECONSTANTS_H

#include <ESP32Time.h>

const int RTC_UTC = 0;
extern ESP32Time rtc;

const int DAYS_IN_WEEK = 7;
const int HOURS_IN_DAY = 24;
const int MINUTES_IN_HOUR = 60;
const int SECONDS_IN_MINUTE = 60;
const int MS_IN_SECOND = 1000;

const int FULL_WEEK_MS = DAYS_IN_WEEK*HOURS_IN_DAY*MINUTES_IN_HOUR*SECONDS_IN_MINUTE*MS_IN_SECOND;

#endif