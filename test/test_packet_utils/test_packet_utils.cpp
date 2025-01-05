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
    const int num_elements = 6;
    std::string packet = "~$9B$RSSI21$21$RSSI37$37$1^test^40672562~";
    std::string expected[] = {"1", "37", "RSSI37", "21", "RSSI21", "9B"};

    for(int i = 0;i<num_elements;++i) {
        TEST_ASSERT_EQUAL_STRING(expected[i].c_str(), getNthLastAdressTableElement(packet, i).c_str());
    }
}

void get_validated_part_correct() {
    std::string packet = "~$9B$RSSI21$21$RSSI37$37$1^test^79c08850~";
    std::string validatedPart = getValidatedPart(packet);
    std::string expectedValidatedPart = "$9B$RSSI21$21$RSSI37$37$1^test^";

    TEST_ASSERT_EQUAL_STRING(expectedValidatedPart.c_str(), validatedPart.c_str());
}

void packet_correct() {
    std::string packet = "~$9B$RSSI21$21$RSSI37$37$1^test^79c08850~";
    TEST_ASSERT_TRUE(isPacketCorrect(packet));

    std::string brokenAdress = "~$9B$RSSI21$22$RSSI37$37$1^test^79c08850~";
    std::string brokenMessage = "~$9B$RSSI21$21$RSSI37$37$1^trol^79c08850~";
    std::string brokenCRC = "~$9B$RSSI21$21$RSSI37$37$1^test^69c08850~";
    TEST_ASSERT_FALSE(isPacketCorrect(brokenAdress));
    TEST_ASSERT_FALSE(isPacketCorrect(brokenMessage));
    TEST_ASSERT_FALSE(isPacketCorrect(brokenCRC));
}

// TODO: Integrity check tests

void setup() {
    UNITY_BEGIN();
    RUN_TEST(create_packet_from_message);
    RUN_TEST(decode_message_from_packet);
    RUN_TEST(nth_last_address_check);
    RUN_TEST(nth_last_address_table_element_check);
    RUN_TEST(get_validated_part_correct);
    RUN_TEST(packet_correct);
    UNITY_END();
}

void loop() {
    // clean stuff up here
}
