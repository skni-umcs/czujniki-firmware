#include <string>
#include "passthroughCommunication.h"
#include <memory>

std::shared_ptr<PassthroughCommunication> PassthroughCommunication::create() {
    auto s = new PassthroughCommunication();
    return std::shared_ptr<PassthroughCommunication>{s};
}