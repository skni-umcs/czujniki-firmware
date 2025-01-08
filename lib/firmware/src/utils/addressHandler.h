#ifndef ADDRESS_HANDLER_H
#define ADDRESS_HANDLER_H

#include <vector>
#include <memory>
#include <exchange/communications/sensorCommunication.h>
#include "time/timer.h"
#include "storageTypes.h"
#include <limits>
#include <memory>

class AddressHandler {
    static std::shared_ptr<AddressHandler> instance;
    moduleAddress address;
    const short ADDRESS_BYTES_START = 0;
    const moduleAddress MIN_MODULE_ADDRESS = 1;
    const moduleAddress MAX_MODULE_ADDRESS = SHRT_MAX-1;
    const char* PREFERENCES_NAMESPACE = "address";
    const char* PREFERENCES_ADDRESS_KEY = "address";

    public:
        static std::shared_ptr<AddressHandler> getInstance();
        static void _DEBUG_setInstance(std::shared_ptr<AddressHandler> addressHandler);
        void saveAddress(moduleAddress address);
        virtual moduleAddress readAddress();
        moduleAddress getAddressFromUser();
        void initializeAddress();
};

#endif