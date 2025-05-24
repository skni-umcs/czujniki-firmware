#include <string>
#include "service_communication.h"
#include <memory>
#include <rom/rtc.h>
#include <sstream>
#include <ArduinoJson.h>
#include "message/message_decode_utils.h"
#include "time/time_constants.h"
#include <message/message_content.h>
#include <utils/logger.h>

std::shared_ptr<ServiceCommunication> ServiceCommunication::create() {
    auto serviceCommunication = std::shared_ptr<ServiceCommunication>(new ServiceCommunication());
    
    serviceCommunication->timeSyncTimer.get()->setExecuteFunction([serviceCommunication]() {
        serviceCommunication->timeSync();
    });
    serviceCommunication->timeSyncTimer.get()->updateTime(serviceCommunication->timeSyncPeriodMs);

    return serviceCommunication;
}

OperationResult ServiceCommunication::getNotified(std::shared_ptr<Message> message) {
    if(!message->getIsPacketCorrect()) {
        Logger::logf("service is notified of invalid packet %s\n", message.get()->getPacket().c_str());
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

    auto message = GeneratedMessage::fromText(packetMessage.getJson(), SERVER_ADDRESS);
    this->transmit(message);
}

OperationResult ServiceCommunication::timeSync() {
    setLastAskTime(DIDNT_ASK);
    Serial.println("bbbbbbbbbbbbbbbbbbbbbb");
    askTimeTimeoutTimer.get()->setExecuteFunction([this]() {
        Serial.println("eeeeeeeeeeeeeeeeeeee");
        askForTime();
    });
    askTimeTimeoutTimer.get()->updateTime(askTimeoutMs);
    return OperationResult::SUCCESS;
}

OperationResult ServiceCommunication::askForTime() {
    this->setLastAskTime(rtc.getEpoch());
    MessageContent packetMessage = MessageContent(TransmissionCode::TIME_SYNCHRONIZATION, "t");
    Logger::log(this->getLastAskTime());
    Logger::log("lastAskTime");
    this->transmit(packetMessage.getJson(), SERVER_ADDRESS);
    return OperationResult::SUCCESS;
}

OperationResult ServiceCommunication::updateTime(unsigned long serverTime) {
    if(lastAskTime == DIDNT_ASK) {
        return OperationResult::OPERATION_IGNORED;
    }

    unsigned long currentTime = rtc.getEpoch();
    unsigned long RTT = currentTime-lastAskTime;
    Logger::logf("I waited from %i to %i with RTT of %i, new time will be %i\n", lastAskTime, currentTime, RTT, serverTime+RTT/2);
    rtc.setTime(serverTime+RTT/2);
    lastAskTime = DIDNT_ASK;
    Logger::logf("Current time after update: %lu\n", rtc.getEpoch());

    return OperationResult::SUCCESS;
}

unsigned long ServiceCommunication::getLastAskTime() {
    return lastAskTime;
}

OperationResult ServiceCommunication::setLastAskTime(unsigned long lastAskTime) {
    this->lastAskTime = lastAskTime;
    return OperationResult::SUCCESS;
}

OperationResult ServiceCommunication::setAskTimeoutMs(int askTimeoutMs) {
    this->askTimeoutMs = askTimeoutMs;
    //TODO: fix timeout timer faulty condition
    // /askTimeTimeoutTimer.get()->updateTime(askTimeoutMs);
    return OperationResult::SUCCESS;
}

OperationResult ServiceCommunication::setTimeSyncPeriodMs(int timeSyncPeriodMs) {
    this->timeSyncPeriodMs = timeSyncPeriodMs;
    timeSyncTimer.get()->updateTime(timeSyncPeriodMs);
    return OperationResult::SUCCESS;
}

int ServiceCommunication::getAskTimeoutMs() {
    return askTimeoutMs;
}

int ServiceCommunication::getTimeSyncPeriodMs() {
    return timeSyncPeriodMs;
}
