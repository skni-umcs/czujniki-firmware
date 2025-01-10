#ifndef TIMER_H
#define TIMER_H

#include <string>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <functional>
#include <memory>

typedef std::function<void()> executeFunctionType;

class Timer : public std::enable_shared_from_this<Timer>
{
    int lastDate = 0; //TODO: remove since it might be useless
    int period = 60;
    bool recentlyUpdated = false;
    executeFunctionType executeFunction = nullptr;
    TaskHandle_t currentTask = NULL;
    private:
        Timer();
    public:
        static std::shared_ptr<Timer> create();
        void changeTimerTask();
        void onTimerUpdate();
        void updateTime(int lastDate, int period);
        executeFunctionType getExecuteFunction();
        void setExecuteFunction(executeFunctionType executeFunction);
        bool getRecentlyUpdated();
        void setRecentlyUpdated(bool recentlyUpdated);
        int getPeriod();

};

#endif