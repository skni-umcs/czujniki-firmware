#include <string>
#include "serviceCommunication.h"
#include <memory>


std::shared_ptr<ServiceCommunication> ServiceCommunication::create() {
    auto s = new ServiceCommunication();
    return std::shared_ptr<ServiceCommunication>{s};
}

