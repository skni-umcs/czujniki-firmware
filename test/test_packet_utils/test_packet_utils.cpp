#include <unity.h>
#include "utils/storageTypes.h"
#include <string>
#include "utils/packetUtils.h"
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <vector>
#include <iostream>

void create_packet_from_message() {
    auto senders = std::vector<moduleAddress>{55};
    Message message = Message(senders, 1, "test");

    std::string out = createPacket(message);

    std::string expected = "~$37$1^test^40672562~";
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), out.c_str());
}

void decode_message_from_packet() {
    std::string packet = "~$37$1^test^40672562~";
    Message out = Message(packet);

    moduleAddress expectedSender = 55;
    moduleAddress expectedDestination = 1;
    std::string expectedMessage = "test";

    TEST_ASSERT_EQUAL(expectedSender, out.getOriginalSender());
    TEST_ASSERT_EQUAL(expectedDestination, out.getDestination());
    TEST_ASSERT_EQUAL_STRING(expectedMessage.c_str(), out.getContent().c_str());
}

void nth_last_address_check() {
    const int num_elements = 4;
    std::string packet = "~$9B$RSSI21$21$RSSI37$37$1^test^40672562~";
    moduleAddress expectedAddresses[] = {1, 55, 33, 155};
    for(int i = 0;i<num_elements;++i) {
        TEST_ASSERT_EQUAL(expectedAddresses[i], nthLastAddress(allAddressTableElements(packet), i));
    }
}

void incorrect_nth_last_address() {
    const int num_elements = 4;
    std::string packet = "~$RSSI0$RSSI21$21$RSSI37$37$1^test^40672562~";
    moduleAddress address = nthLastAddress(allAddressTableElements(packet), 3);
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, address);
}

void something_that_isnt_even_packet() {
    std::string packet = "~s$ema test ~_~";
    Message message = Message(packet);
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, message.getOriginalSender());
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, message.getDestination());
    TEST_ASSERT_EQUAL_STRING("", message.getContent().c_str());

    TEST_ASSERT_FALSE(isPacketCorrect(packet));
}

void only_crc() {
    std::string packet = "^~";
    Message message = Message(packet);
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, message.getOriginalSender());
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, message.getDestination());
    TEST_ASSERT_EQUAL_STRING("", message.getContent().c_str());

    TEST_ASSERT_FALSE(isPacketCorrect(packet));
}

void random_corruption() {
    std::string packet = "~$9Be&!*&$%!JSAFT@!%!&#HFA(T#@QUNst^40672562~";
    Message message = Message(packet);
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, message.getOriginalSender());
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, message.getDestination());
    TEST_ASSERT_EQUAL_STRING("", message.getContent().c_str());

    TEST_ASSERT_FALSE(isPacketCorrect(packet));
}

void random_corruption_but_crc_is_correct() {
    std::string packet = "~$9Be&!*&$%!JSAFT@!%!&#HFA(T#@QUNst^0dab27ed~";
    Message message = Message(packet);
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, message.getOriginalSender());
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, message.getDestination());
    TEST_ASSERT_EQUAL_STRING("", message.getContent().c_str());

    TEST_ASSERT_FALSE(isPacketCorrect(packet));
}

void nth_last_address_table_element_check() {
    const int num_elements = 6;
    std::string packet = "~$9B$RSSI21$21$RSSI37$37$1^test^40672562~";
    std::string expected[] = {"1", "37", "RSSI37", "21", "RSSI21", "9B"};

    for(int i = 0;i<num_elements;++i) {
        TEST_ASSERT_EQUAL_STRING(expected[i].c_str(), nthLastAddressTableElement(allAddressTableElements(packet), i).c_str());
    }
}

void get_all_senders_correctly() {
    std::string packet = "~HOP$100$RSSI9$9$RSSIffff$ffff$RSSIf8$f8$1^test^79c08850~";
    Message message = Message(packet);

    std::vector<moduleAddress> expectedSenders = {248, 32767, 9, 256};
    std::vector<moduleAddress> out = getSenders(allAddressTableElements(packet));
    for(int i = 0;i<expectedSenders.size();++i) {
        TEST_ASSERT_EQUAL(expectedSenders.at(i), out.at(i));
    }

}

void get_validated_part_correct() {
    std::string packet = "~$9B$RSSI21$21$RSSI37$37$1^test^79c08850~";
    std::string validatedPart = getValidatedPart(packet);
    std::string expectedValidatedPart = "$9B$RSSI21$21$RSSI37$37$1^test^";

    TEST_ASSERT_EQUAL_STRING(expectedValidatedPart.c_str(), validatedPart.c_str());
}

void crc_correct() {
    std::string packet = "~$9B$RSSI21$21$RSSI37$37$1^test^79c08850~";
    TEST_ASSERT_TRUE(isCrcCorrect(packet));

    std::string brokenAdress = "~$9B$RSSI21$22$RSSI37$37$1^test^79c08850~";
    std::string brokenMessage = "~$9B$RSSI21$21$RSSI37$37$1^trol^79c08850~";
    std::string brokenCRC = "~$9B$RSSI21$21$RSSI37$37$1^test^69c08850~";
    TEST_ASSERT_FALSE(isCrcCorrect(brokenAdress));
    TEST_ASSERT_FALSE(isCrcCorrect(brokenMessage));
    TEST_ASSERT_FALSE(isCrcCorrect(brokenCRC));
}

void packet_correct() {
    std::string packet = "~$9B$RSSI21$21$RSSI37$37$1^test^79c08850~";
    TEST_ASSERT_TRUE(isPacketCorrect(packet));

    std::string leftBorder = "/$9B$RSSI21$21$RSSI37$37$1^test^79c08850~";
    std::string rightBorder = "~$9B$RSSI21$21$RSSI37$37$1^test^79c0885~0";
    std::string inside = "~$9B$RSSI21$21$RSSI37$37$1@test^79c08850~";
    TEST_ASSERT_FALSE(isPacketCorrect(leftBorder));
    TEST_ASSERT_FALSE(isPacketCorrect(rightBorder));
    TEST_ASSERT_FALSE(isPacketCorrect(inside));
}

void also_accept_hops() {
    std::string packet = "~$30$9B$RSSI21$21$RSSI37$37$1^test^450d3fed~";
    TEST_ASSERT_TRUE(isPacketCorrect(packet));
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(create_packet_from_message);
    RUN_TEST(decode_message_from_packet);
    RUN_TEST(something_that_isnt_even_packet);
    RUN_TEST(only_crc);
    RUN_TEST(random_corruption);
    RUN_TEST(random_corruption_but_crc_is_correct);
    RUN_TEST(also_accept_hops);
    RUN_TEST(incorrect_nth_last_address);
    RUN_TEST(nth_last_address_check);
    RUN_TEST(nth_last_address_table_element_check);
    RUN_TEST(get_validated_part_correct);
    RUN_TEST(crc_correct);
    RUN_TEST(packet_correct);
    UNITY_END();
}

void loop() {
    // clean stuff up here
}
