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
    int periodMs = 60000;
    executeFunctionType executeFunction = nullptr;
    TaskHandle_t currentTask = NULL;
    int taskPriority;
    private:
        Timer();
    public:
        static std::shared_ptr<Timer> create(int taskPriority = 1);
        void changeTimerTask();
        void updateTime(int period);
        executeFunctionType getExecuteFunction();
        void setExecuteFunction(executeFunctionType executeFunction);
        int getPeriodMs();

};

#endif