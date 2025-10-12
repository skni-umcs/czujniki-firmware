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
#include <utils/address_handler.h>
#include "utils/preferences_constant.h"

const char* LAST_REMOTE_RESET_TIMESTAMP = "lastRemoteResetTimestamp";

std::shared_ptr<ConfigCommunication> ConfigCommunication::create(std::shared_ptr<ConfigurationFacade> configFacade) {
    auto configCommunication = std::shared_ptr<ConfigCommunication>(new ConfigCommunication());
    preferences.begin(LAST_REMOTE_RESET_TIMESTAMP);

    configCommunication->configFacade = configFacade;

    return configCommunication;
}

OperationResult ConfigCommunication::getNotified(std::shared_ptr<Message> message) {
    if(!message->getIsPacketCorrect()) {
        Logger::logf("config is notified of invalid packet %s\n", message.get()->getPacket().c_str());
        return OperationResult::ERROR;
    }
    //No current destination check - configs are global and broadcasted unless specified otherwise in the updateConfig switch case
    MessageContent serverMessage = MessageContent::fromJson(message->getContent());
    TransmissionCode messageType = serverMessage.getType();
    switch(messageType) {
        case(TransmissionCode::CONFIG_UPDATE):
            updateConfig(serverMessage, message);
            break;
    }
    return OperationResult::SUCCESS;
}

bool isBroadcastOrCurrentDestination(char reach, moduleAddress destination) {
    return reach == '*' || destination == AddressHandler::getInstance()->readAddress();
}

OperationResult ConfigCommunication::updateConfig(MessageContent messageContent, std::shared_ptr<Message> message) {
    std::string configDetails = messageContent.getDetails();
    if (configDetails.empty()) {
        return OperationResult::ERROR;
    }
    moduleAddress destination = message->getDestination();

    char reach = static_cast<char>(std::tolower(static_cast<unsigned char>(configDetails[0])));
    char cmd = static_cast<char>(std::tolower(static_cast<unsigned char>(configDetails[1])));
    std::string arg = configDetails.substr(2);

    auto ltrim = [](std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    };
    auto rtrim = [](std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
    };
    ltrim(arg);
    rtrim(arg);

    try {
        if(isBroadcastOrCurrentDestination(reach, destination)) {
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
                    if(preferences.isKey(LAST_REMOTE_RESET_TIMESTAMP) && preferences.getUShort(LAST_REMOTE_RESET_TIMESTAMP) != messageContent.getJsonificationEpoch()) {
                        preferences.putUShort(LAST_REMOTE_RESET_TIMESTAMP, messageContent.getJsonificationEpoch());
                        ESP.restart();
                    }
                    return OperationResult::SUCCESS;
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
        }
        else {
            return OperationResult::OPERATION_IGNORED;
        }
     
    } catch (const std::exception &) {
        return OperationResult::ERROR;
    }
}

