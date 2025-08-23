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

std::shared_ptr<ConfigCommunication> ConfigCommunication::create(std::shared_ptr<ConfigurationFacade> configFacade) {
    auto configCommunication = std::shared_ptr<ConfigCommunication>(new ConfigCommunication());

    configCommunication->configFacade = configFacade;

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
        case(TransmissionCode::CONFIG_UPDATE):
            updateConfig(message->getContent());
            break;
    }
    return OperationResult::SUCCESS;
}
OperationResult ConfigCommunication::updateConfig(std::string configDetails) {
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
            case 'a': {
                if (arg.empty()) return OperationResult::ERROR;
                int seconds = std::stoi(arg);
                return configFacade->setAskTimeoutMs(seconds*1000);
            }

            case 'p': {
                if (arg.empty()) return OperationResult::ERROR;
                int seconds = std::stoi(arg);
                return configFacade->setTimeSyncPeriodMs(seconds*1000);
            }

            case 'r': {
                //todo: implement
                return OperationResult::NOT_FOUND;
            }

            case 't': {
                if (arg.empty()) return OperationResult::ERROR;
                int seconds = std::stoi(arg);
                return configFacade->setTelemetryPeriodMs(seconds*1000);
            }

            case 's': {
                if (arg.empty()) return OperationResult::ERROR;
                int seconds = std::stoi(arg);
                return configFacade->setServicePeriodMs(seconds*1000);
            }

            default:
                return OperationResult::ERROR;
        }
    } catch (const std::exception &) {
        return OperationResult::ERROR;
    }
}

