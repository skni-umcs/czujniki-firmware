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

void incorrect_nth_last_address() {
    const int num_elements = 4;
    std::string packet = "~$RSSI0$RSSI21$21$RSSI37$37$1^test^40672562~";
    moduleAddress address = getNthLastAddress(packet, 3);
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, address);
}

void something_that_isnt_even_packet() {
    std::string packet = "~s$ema test ~_~";
    Message message = getPacketMessage(packet);
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, message.sender);
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, message.destination);
    TEST_ASSERT_EQUAL_STRING("", message.content.c_str());

    TEST_ASSERT_FALSE(isPacketCorrect(packet));
}

void only_crc() {
    std::string packet = "^~";
    Message message = getPacketMessage(packet);
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, message.sender);
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, message.destination);
    TEST_ASSERT_EQUAL_STRING("", message.content.c_str());

    TEST_ASSERT_FALSE(isPacketCorrect(packet));
}

void random_corruption() {
    std::string packet = "~$9Be&!*&$%!JSAFT@!%!&#HFA(T#@QUNst^40672562~";
    Message message = getPacketMessage(packet);
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, message.sender);
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, message.destination);
    TEST_ASSERT_EQUAL_STRING("", message.content.c_str());

    TEST_ASSERT_FALSE(isPacketCorrect(packet));
}

void random_corruption_but_crc_is_correct() {
    std::string packet = "~$9Be&!*&$%!JSAFT@!%!&#HFA(T#@QUNst^0dab27ed~";
    Message message = getPacketMessage(packet);
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, message.sender);
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, message.destination);
    TEST_ASSERT_EQUAL_STRING("", message.content.c_str());

    TEST_ASSERT_FALSE(isPacketCorrect(packet));
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
