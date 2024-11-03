#ifndef TIMER_H
#define TIMER_H

#include <string>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

typedef void (*executeFunctionType)();

class Timer
{
    int lastDate = 0; //TODO: remove since it might be useless
    int period = 60;
    bool recentlyUpdated = false;
    executeFunctionType executeFunction = nullptr;
    TaskHandle_t currentTask = NULL;
    public:
        void changeTimerTask();
        void onTimerUpdate();
        void updateTime(int lastDate, int period);
        executeFunctionType getExecuteFunction();
        bool getRecentlyUpdated();
        void setRecentlyUpdated(bool recentlyUpdated);
        int getPeriod();
};

#endif