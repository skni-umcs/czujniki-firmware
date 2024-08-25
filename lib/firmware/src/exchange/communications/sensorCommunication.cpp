#include <string>
#include "sensorCommunication.h"
#include <memory>


std::shared_ptr<SensorCommunication> SensorCommunication::create() {
    auto s = new SensorCommunication();
    return std::shared_ptr<SensorCommunication>{s};
}