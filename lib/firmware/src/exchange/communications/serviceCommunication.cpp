#include <string>
#include "serviceCommunication.h"
#include <memory>
#include <rom/rtc.h>
#include <sstream>
#include <ArduinoJson.h>
#include "utils/packetUtils.h"
#include "time/timeConstants.h"
#include <time/timerUpdate.h>

static int ASK_TIMEOUT_MS = 10000;

int startTimestamp = 0;
int coile = 20;

int predictedMessages(int timestamp) {
  int difference = timestamp-startTimestamp;
  return difference/coile;

}

std::shared_ptr<ServiceCommunication> ServiceCommunication::create() {
    auto s = new ServiceCommunication();
    return std::shared_ptr<ServiceCommunication>{s};
}

OperationResult ServiceCommunication::getNotified(std::shared_ptr<Message> message) {
    if(!message->getIsPacketCorrect()) {
        Serial.printf("service is notified of invalid packet %s\n", message.get()->getPacket().c_str());
        return OperationResult::ERROR;
    }
    if(!message->isCurrentDestination()) {
        return OperationResult::OPERATION_IGNORED;
    }
    PacketMessage serverMessage = PacketMessage::fromJson(message->getContent());
    TransmissionCode messageType = serverMessage.getType();
    switch(messageType) {
        case(TransmissionCode::TIME_SYNCHRONIZATION):
            updateTime(serverMessage.getJsonificationEpoch());
            break;
    }
    return OperationResult::SUCCESS;
}

void ServiceCommunication::sendResetReason() {
    JsonDocument doc;
    JsonArray messages = doc.to<JsonArray>();
    std::string serializedJson;

    int reset_reasons[] = {
        rtc_get_reset_reason(0), 
        rtc_get_reset_reason(1)
    };
    for (const auto& reason : reset_reasons) {
        messages.add(reason);
    }
    serializeJson(doc, serializedJson);
    PacketMessage packetMessage = PacketMessage(TransmissionCode::RESET, serializedJson);

    this->transmit(packetMessage.getJson(), SERVER_ADDRESS);
}

void askForTimeTask(void* object) {
    std::shared_ptr<ServiceCommunication>* serviceCommunication = static_cast<std::shared_ptr<ServiceCommunication>*>(object);
    while(serviceCommunication->get()->getLastAskTime() != DIDNT_ASK) {
        serviceCommunication->get()->setLastAskTime(rtc.getEpoch());
        PacketMessage packetMessage = PacketMessage(TransmissionCode::TIME_SYNCHRONIZATION, "t");
        Serial.println(serviceCommunication->get()->getLastAskTime());
        Serial.println("lastAskTime");
        serviceCommunication->get()->transmit(packetMessage.getJson(), SERVER_ADDRESS);
        vTaskDelay(ASK_TIMEOUT_MS);
        Serial.println("Ask for time timeout, checking if time updated");
    }
    vTaskDelete(NULL);
}

OperationResult ServiceCommunication::setAskForTimeTask() {
    auto* taskPtr = new std::shared_ptr<Communication>(shared_from_this());

    const int bytesNeeded = 25600; //temporary value thats working
    const char* taskName = "askForTimeTask";
    void* taskArgument = static_cast<void*>(taskPtr);
    const int taskPriority = 2;
    TaskHandle_t* const taskHandle = NULL;
    xTaskCreate(askForTimeTask, taskName, bytesNeeded, taskArgument, taskPriority, taskHandle);
    return OperationResult::SUCCESS;
}

OperationResult ServiceCommunication::askForTime() {
    lastAskTime = rtc.getEpoch();
    setAskForTimeTask();
    return OperationResult::SUCCESS;
}

OperationResult ServiceCommunication::updateTime(unsigned long serverTime) {
    if(lastAskTime == DIDNT_ASK) {
        return OperationResult::OPERATION_IGNORED;
    }

    unsigned long currentTime = rtc.getEpoch();
    unsigned long RTT = currentTime-lastAskTime;
    Serial.printf("I waited from %i to %i with RTT of %i, new time will be %i\n", lastAskTime, currentTime, RTT, serverTime+RTT/2);
    TimerUpdate::setTime(serverTime+RTT/2);
    lastAskTime = DIDNT_ASK;
    Serial.printf("Current time after update: %lu\n", rtc.getEpoch());

    startTimestamp = rtc.getEpoch();
    auto checkTimer = Timer::create();
    Serial.println("Created prediction timer");
    int periodS = 20;
    checkTimer->setExecuteFunction([this](){
        this->transmit("Predicted number of sent messages: "+std::to_string(predictedMessages(rtc.getEpoch())), 0);
    });
    checkTimer->updateTime(periodS*1000);

    return OperationResult::SUCCESS;
}

unsigned long ServiceCommunication::getLastAskTime() {
    return lastAskTime;
}

OperationResult ServiceCommunication::setLastAskTime(unsigned long lastAskTime) {
    this->lastAskTime = lastAskTime;
    return OperationResult::SUCCESS;
}