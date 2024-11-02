#ifndef TIMER_H
#define TIMER_H

#include <string>

class Timer
{
    int lastDate = 0;
    int period = 60;
    bool recentlyUpdated = false;
    void (*executeFunction)() = nullptr;

    public:
        void onTimerUpdate();
        void onTaskDelay();
        void updateTime(int lastDate, int period);
};

#endif