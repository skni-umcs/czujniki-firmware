#ifndef CONFIGCOMMUNICATION_H
#define CONFIGCOMMUNICATION_H

#include <vector>
#include "small_communication.h"
#include <exchange/transmits/small_transmit.h>
#include "time/timer.h"
#include "time/time_constants.h"
#include <sensors/sensor_facade.h>

class ConfigCommunication : public SmallCommunication {

    std::shared_ptr<SensorFacade> sensorFacade;
    std::shared_ptr<ServiceCommunication> serviceCommunication;

    protected:
        OperationResult setAskForTimeTask();
        ConfigCommunication() {};

    public:
        static std::shared_ptr<ConfigCommunication> create(std::shared_ptr<SensorFacade> sensorFacade, std::shared_ptr<ServiceCommunication> serviceCommunication);
        OperationResult getNotified(std::shared_ptr<Message> message) override;
        OperationResult modifyConfig(std::string);

};

#endif