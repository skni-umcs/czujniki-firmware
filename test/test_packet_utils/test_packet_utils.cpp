#include <unity.h>
#include "utils/storageTypes.h"
#include <string>
#include "utils/packetUtils.h"
#include <Adafruit_I2CDevice.h>
#include <SPI.h>

void create_packet_from_message() {
    Message message = Message();
    message.sender = 55;
    message.destination = 1;
    message.content = "test";

    std::string out = createPacket(message);

    std::string expected = "~$37$1^test^40672562~";
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), out.c_str());
}

void decode_message_from_packet() {
    std::string packet = "~$37$1^test^40672562~";
    Message out = getPacketMessage(packet);

    moduleAddress expectedSender = 55;
    moduleAddress expectedDestination = 1;
    std::string expectedMessage = "test";

    TEST_ASSERT_EQUAL(expectedSender, out.sender);
    TEST_ASSERT_EQUAL(expectedDestination, out.destination);
    TEST_ASSERT_EQUAL_STRING(expectedMessage.c_str(), out.content.c_str());
}

void nth_last_address_check() {
    const int num_elements = 4;
    std::string packet = "~$9B$RSSI21$21$RSSI37$37$1^test^40672562~";
    moduleAddress expectedAddresses[] = {1, 55, 33, 155};
    for(int i = 0;i<num_elements;++i) {
        TEST_ASSERT_EQUAL(expectedAddresses[i], getNthLastAddress(packet, i));
    }
}

void nth_last_address_table_element_check() {
    std::string packet = "~$9B$RSSI21$21$RSSI37$37$1^test^40672562~";
    std::string out = getNthLastAdressTableElement(packet, 4);
    std::string expected = "RSSI21";

    TEST_ASSERT_EQUAL_STRING(expected.c_str(), out.c_str());
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(create_packet_from_message);
    RUN_TEST(decode_message_from_packet);
    RUN_TEST(nth_last_address_check);
    RUN_TEST(nth_last_address_table_element_check);
    UNITY_END();
}

void loop() {
    // clean stuff up here
}
