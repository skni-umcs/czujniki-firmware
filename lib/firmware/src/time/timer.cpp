#include "timer.h"
#include <iostream>
#include "timerUpdate.h"

Timer::Timer() {}

std::shared_ptr<Timer> Timer::create(int taskPriority) {
    auto timerPtr = std::shared_ptr<Timer>(new Timer());
    timerPtr->taskPriority = taskPriority;
    TimerUpdate::addSubscriber(timerPtr);
    return timerPtr;
}

void timerTask(void* timerObjectRawPointer) {
    std::shared_ptr<Timer>* timerPtr = static_cast<std::shared_ptr<Timer>*>(timerObjectRawPointer);
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    auto condition = timerPtr->get()->getTimerCondition();
    while(condition == nullptr || condition()) {
        if(timerPtr->get()->getExecuteFunction() != nullptr && !timerPtr->get()->getRecentlyUpdated()) {
            timerPtr->get()->getExecuteFunction()();
        }
        timerPtr->get()->setRecentlyUpdated(false);
        vTaskDelayUntil(&xLastWakeTime, timerPtr->get()->getPeriodMs());
    }
}

void Timer::changeTimerTask() {
    if(this->currentTask != NULL) {
        vTaskDelete(this->currentTask);
        this->currentTask = NULL;
    }

    auto* taskPtr = new std::shared_ptr<Timer>(shared_from_this());

    const int bytesNeeded = 25600; //temporary value thats working
    const char* taskName = "timerTask";
    void* taskArgument = static_cast<void*>(taskPtr);
    TaskHandle_t* const taskHandle = &this->currentTask;
    xTaskCreate(timerTask, taskName, bytesNeeded, taskArgument, taskPriority, taskHandle);
}

void Timer::updateTime(int period) {
    this->recentlyUpdated = true;
    this->periodMs = period;
    this->changeTimerTask();
}

void Timer::onTimerUpdate() {
    this->recentlyUpdated = true;
}

executeFunctionType Timer::getExecuteFunction() {
    return this->executeFunction;
}

void Timer::setExecuteFunction(executeFunctionType executeFunction) {
    this->executeFunction = executeFunction;
}

timerConditionType Timer::getTimerCondition() {
    return this->timerCondition;
}

void Timer::setTimerCondition(timerConditionType timerCondition) {
    this->timerCondition = timerCondition;
}

bool Timer::getRecentlyUpdated() {
    return this->recentlyUpdated;
}

void Timer::setRecentlyUpdated(bool recentlyUpdated) {
    this->recentlyUpdated = recentlyUpdated;
}

int Timer::getPeriodMs() {
    return this->periodMs;
}