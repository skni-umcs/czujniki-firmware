#include "sensor.h"
#include <vector>
#include <exchange/transmits/jsonTransmit.h>
#include "communication.h"
#include <memory>

class JsonCommunication : Communication {
    protected:
        std::vector<std::unique_ptr<JsonTransmit>> transmitTo;
    public:
        std::string getNotified(std::string message) = 0;
        virtual void subscribe(std::unique_ptr<JsonTransmit> transmit) = 0;
        virtual void unsubscribe(std::unique_ptr<JsonTransmit> transmit) = 0;
};