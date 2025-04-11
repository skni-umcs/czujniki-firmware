#include <string>
#ifndef JSON_MESSAGE_H
#define JSON_MESSAGE_H

enum class TransmissionCode : char {
    //error
    ERROR_CODE = ' ',

    //JSON codes
    MESSAGE_TYPE = 't',
    MESSAGE = 'm',
    TIMESTAMP = 'c',

    //message types
    RESET = 'r',
    SENSOR_READING = 'p',
    TIME_SYNCHRONIZATION = 'o'
};

class JsonMessage {
    TransmissionCode type;
    std::string message;
    unsigned long jsonificationEpoch;

    public:
        JsonMessage(TransmissionCode type, std::string message);
        std::string getJson();
        static JsonMessage fromJson(std::string jsonString);
        TransmissionCode getType();
        std::string getMessage();
        unsigned long getJsonificationEpoch();
};

static std::string transmissionCodeFromEnum(TransmissionCode transmissionCode);

#endif