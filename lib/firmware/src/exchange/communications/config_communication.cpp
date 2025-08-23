#include <string>
#include "config_communication.h"
#include <memory>
#include <rom/rtc.h>
#include <sstream>
#include <ArduinoJson.h>
#include "message/message_decode_utils.h"
#include "time/time_constants.h"
#include <message/message_content.h>
#include <utils/logger.h>

std::shared_ptr<ConfigCommunication> ConfigCommunication::create(std::shared_ptr<SensorFacade> sensorFacade, std::shared_ptr<ServiceCommunication> serviceCommunication) {
    auto configCommunication = std::shared_ptr<ConfigCommunication>(new ConfigCommunication());

    configCommunication->sensorFacade = sensorFacade;
    configCommunication->serviceCommunication = serviceCommunication;

    return configCommunication;
}

OperationResult ConfigCommunication::getNotified(std::shared_ptr<Message> message) {
    if(!message->getIsPacketCorrect()) {
        Logger::logf("config is notified of invalid packet %s\n", message.get()->getPacket().c_str());
        return OperationResult::ERROR;
    }
    if(!message->isCurrentDestination()) {
        return OperationResult::OPERATION_IGNORED;
    }
    MessageContent serverMessage = MessageContent::fromJson(message->getContent());
    TransmissionCode messageType = serverMessage.getType();
    switch(messageType) {
        case(TransmissionCode::CONFIG_DATA):
            modifyConfig(message->getContent());
            break;
    }
    return OperationResult::SUCCESS;
}

OperationResult ConfigCommunication::modifyConfig(std::string configDetails) {
    if (configDetails.empty()) {
        return OperationResult::ERROR;
    }

    char cmd = static_cast<char>(std::tolower(static_cast<unsigned char>(configDetails[0])));
    std::string arg = configDetails.substr(1);

    auto ltrim = [](std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    };
    auto rtrim = [](std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
    };
    ltrim(arg);
    rtrim(arg);

    try {
        switch (cmd) {
            case 'a': { // askTimeoutMs (ms)
                if (arg.empty()) return OperationResult::ERROR;
                int val = std::stoi(arg);
                return serviceCommunication->setAskTimeoutMs(val*1000);
            }

            case 'p': { // timeSyncPeriodMs (ms)
                if (arg.empty()) return OperationResult::ERROR;
                int val = std::stoi(arg);
                return serviceCommunication->setTimeSyncPeriodMs(val*1000);
            }

            case 'r': { // reset true/false or 1/0
                return OperationResult::NOT_FOUND;
            }

            case 't': { // frequency danych klimatycznych (seconds)
                if (arg.empty()) return OperationResult::ERROR;
                int seconds = std::stoi(arg);
                // Assumes a setter exists in the class; user said they'll provide other functions.
                // Example expected function name: setClimateFrequencySeconds(int)
                return sensorFacade->setTelemetryPeriodMs(seconds*1000);
            }

            case 's': { // frequency danych serwisowych (seconds)
                if (arg.empty()) return OperationResult::ERROR;
                int seconds = std::stoi(arg);
                // Assumes a setter exists in the class; user said they'll provide other functions.
                // Example expected function name: setServiceFrequencySeconds(int)
                return sensorFacade->setServicePeriodMs(seconds*1000);
            }

            default:
                return OperationResult::NOT_FOUND;
        }
    } catch (const std::exception &) {
        return OperationResult::ERROR;
    }
}