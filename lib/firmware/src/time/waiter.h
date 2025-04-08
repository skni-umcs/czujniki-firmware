#ifndef WAITER_H
#define WAITER_H

#include <string>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <functional>
#include <memory>
#include <utils/operation_result.h>

typedef std::function<void()> executeFunctionType;
typedef std::function<bool()> timerConditionType;

class Waiter : public std::enable_shared_from_this<Waiter>
{
    int waitMS = 60000;
    executeFunctionType executeFunction = nullptr;
    timerConditionType timerCondition = nullptr;
    TaskHandle_t currentTask = NULL;
    int taskPriority;
    private:
        Waiter();
    public:
        static std::shared_ptr<Waiter> create(int taskPriority = 1);
        void changeTimerTask();
        void onTimerUpdate();
        void updateTimeNoSkip(int period);
        void updateTime(int period);
        executeFunctionType getExecuteFunction();
        void setExecuteFunction(executeFunctionType executeFunction);
        timerConditionType getTimerCondition();
        void setTimerCondition(timerConditionType timerCondition);
        bool getRecentlyUpdated();
        void setRecentlyUpdated(bool recentlyUpdated);
        int getWaitMs();
        OperationResult deleteTimerTask();

};

#endif