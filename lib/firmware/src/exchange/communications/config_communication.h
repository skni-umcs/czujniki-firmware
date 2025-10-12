#ifndef ConfigCommunication_H
#define ConfigCommunication_H

#include <vector>
#include "small_communication.h"
#include <exchange/transmits/small_transmit.h>
#include "time/timer.h"
#include "time/time_constants.h"
#include <utils/configuration_facade.h>

class ConfigCommunication : public SmallCommunication {
    std::shared_ptr<ConfigurationFacade> configFacade;

    protected:
        ConfigCommunication() {};

    public:
        static std::shared_ptr<ConfigCommunication> create(std::shared_ptr<ConfigurationFacade> configFacade);
        OperationResult getNotified(std::shared_ptr<Message> message) override;
        OperationResult updateConfig(MessageContent messageContent, std::shared_ptr<Message> message);

};

#endif