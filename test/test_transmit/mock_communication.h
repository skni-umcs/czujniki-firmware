#ifndef MOCKCOMMUNICATION_H
#define MOCKCOMMUNICATION_H

#include <vector>
#include "exchange/communications/small_communication.h"
#include "exchange/communications/big_communication.h"

class MockCommunication : public BigCommunication, public SmallCommunication {
    
    public:
        static std::shared_ptr<MockCommunication> create();
        OperationResult getNotified(std::shared_ptr<Message> message) override;
};

#endif