#include <string>
#ifndef message_content_H
#define message_content_H

enum class TransmissionCode : char {
    //error
    ERROR_CODE = ' ',

    //JSON codes
    MESSAGE_TYPE = 't',
    DETAILS = 'm',
    TIMESTAMP = 'c',

    //message types
    RESET = 'r',
    TELEMETRY_READING = 'p',
    SERVICE_READING = 's',
    TIME_SYNCHRONIZATION = 'o',
    CONFIG_DATA = 'u'
};

class MessageContent {
    TransmissionCode type;
    std::string details;
    unsigned long jsonificationEpoch;

    public:
        MessageContent(TransmissionCode type, std::string message);
        std::string getJson();
        static MessageContent fromJson(std::string jsonString);
        TransmissionCode getType();
        std::string getDetails();
        unsigned long getJsonificationEpoch();
};

static std::string transmissionCodeFromEnum(TransmissionCode transmissionCode);

#endif