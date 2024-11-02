#ifndef TIMER_H
#define TIMER_H

#include <string>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

class Timer
{
    public:
        int lastDate = 0;
        int period = 60;
        bool recentlyUpdated = false;
        void (*executeFunction)() = nullptr;
        TaskHandle_t currentTask = NULL;
        void onTimerUpdate();
        void updateTime(int lastDate, int period);
};

#endif