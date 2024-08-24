#include "sensor.h"
#include <vector>
#include <exchange/transmits/bigTransmit.h>

class BigCommunication : Communication {
    std::vector<std::unique_ptr<BigTransmit>> transmitTo;
    public:
        std::string getNotified(std::string message) override;
        virtual std::string subscribe(std::unique_ptr<BigTransmit> bigTransmit) = 0;
        virtual std::string unsubscribe(std::unique_ptr<BigTransmit> bigTransmit) = 0;
};
