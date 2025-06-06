#include "waiter.h"
#include <iostream>
#include <HardwareSerial.h>
#include <utils/logger.h>

Waiter::Waiter() {}

std::shared_ptr<Waiter> Waiter::create(int taskPriority) {
    auto timerPtr = std::shared_ptr<Waiter>(new Waiter());
    timerPtr->taskPriority = taskPriority;
    return timerPtr;
}

void waiterTask(void* timerObjectRawPointer) {
    std::shared_ptr<Waiter>* timerPtr = static_cast<std::shared_ptr<Waiter>*>(timerObjectRawPointer);
    while(true) {
        vTaskDelay(timerPtr->get()->getWaitMs());
        if(timerPtr->get()->getExecuteFunction() != nullptr) {
            timerPtr->get()->getExecuteFunction()();
        }
    }
    timerPtr->get()->deleteTimerTask();
}

void Waiter::changeTimerTask() {
    deleteTimerTask();

    auto* taskPtr = new std::shared_ptr<Waiter>(shared_from_this());

    const int bytesNeeded = 2560*2; //temporary value thats working
    const char* taskName = "timerTask";
    void* taskArgument = static_cast<void*>(taskPtr);
    TaskHandle_t* const taskHandle = &this->currentTask;
    int heap_status = xTaskCreate(waiterTask, taskName, bytesNeeded, taskArgument, taskPriority, taskHandle);
    if(heap_status != 1) {
        Logger::log("HEAP OVERLOAD");
    }
}

void Waiter::updateTime(int period) {
    this->waitMS = period;
    //changeTimerTask();
}

executeFunctionType Waiter::getExecuteFunction() {
    return this->executeFunction;
}

void Waiter::setExecuteFunction(executeFunctionType executeFunction) {
    this->executeFunction = executeFunction;
}

int Waiter::getWaitMs() {
    return this->waitMS;
}

OperationResult Waiter::deleteTimerTask() {
    if(this->currentTask != NULL) {
        vTaskDelete(this->currentTask);
        this->currentTask = NULL;
        return OperationResult::SUCCESS;
    }
    return OperationResult::NOT_FOUND;
}