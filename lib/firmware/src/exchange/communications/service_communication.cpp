#include <string>
#include "service_communication.h"
#include <memory>
#include <rom/rtc.h>
#include <sstream>
#include <ArduinoJson.h>
#include "message/message_decode_utils.h"
#include "time/time_constants.h"
#include <time/timer_update.h>
#include <message/message_content.h>

static int ASK_TIMEOUT_MS = 10000;
int startTimestamp = 0;
int coile = 32;

int predictedMessages(int timestamp) {
  int difference = timestamp-startTimestamp;
  return difference/coile;

}

std::shared_ptr<ServiceCommunication> ServiceCommunication::create() {
    auto serviceCommunication = std::shared_ptr<ServiceCommunication>(new ServiceCommunication());

    serviceCommunication->setLastAskTime(DIDNT_ASK);
    serviceCommunication->askTimeTimeoutTimer.get()->setExecuteFunction([serviceCommunication]() {
        serviceCommunication->askForTime();
    });
    serviceCommunication->askTimeTimeoutTimer.get()->setTimerCondition([serviceCommunication]() {
        Serial.printf("last ask time %d \n",  serviceCommunication->getLastAskTime());
        return serviceCommunication->getLastAskTime() != DIDNT_ASK;
    });
     
    serviceCommunication->askTimeTimeoutTimer.get()->updateTime(ASK_TIMEOUT_MS);

    return serviceCommunication;
}

OperationResult ServiceCommunication::getNotified(std::shared_ptr<Message> message) {
    if(!message->getIsPacketCorrect()) {
        Serial.printf("service is notified of invalid packet %s\n", message.get()->getPacket().c_str());
        return OperationResult::ERROR;
    }
    if(!message->isCurrentDestination()) {
        return OperationResult::OPERATION_IGNORED;
    }
    MessageContent serverMessage = MessageContent::fromJson(message->getContent());
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
    MessageContent packetMessage = MessageContent(TransmissionCode::RESET, serializedJson);

    this->transmit(packetMessage.getJson(), SERVER_ADDRESS);
}

OperationResult ServiceCommunication::askForTime() {
    this->setLastAskTime(rtc.getEpoch());
    MessageContent packetMessage = MessageContent(TransmissionCode::TIME_SYNCHRONIZATION, "t");
    Serial.println(this->getLastAskTime());
    Serial.println("lastAskTime");
    this->transmit(packetMessage.getJson(), SERVER_ADDRESS);
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
    int periodS = coile;
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