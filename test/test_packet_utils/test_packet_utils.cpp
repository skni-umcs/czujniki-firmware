#include <unity.h>
#include "utils/storageTypes.h"
#include <string>
#include "utils/packetUtils.h"
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <vector>
#include <iostream>
#include "utils/addressHandler.h"

const moduleAddress TEST_MODULE_ADDRESS = 28;

class MockAddressHandler : public AddressHandler {
    public:
        moduleAddress readAddress() override;
};

moduleAddress MockAddressHandler::readAddress() {
    return TEST_MODULE_ADDRESS;
}

void setupAddressHandler() {
    AddressHandler::_DEBUG_setInstance(std::shared_ptr<AddressHandler>(new MockAddressHandler()));
}

void create_packet_from_message() {
    auto senders = std::vector<moduleAddress>{55};
    auto rssi = std::vector<std::string>();
    int hopLimit = 4;
    LoraMessage message = LoraMessage(senders, 1, "test", rssi, hopLimit, 0, 0);
    
    std::string out = message.createPacket();

    std::string expected = "~$4$37$1^test^407cfe14~";
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), out.c_str());
}

void create_packet_from_message_multiple_senders() {
    auto senders = std::vector<moduleAddress>{55, 133, 28};
    auto rssi = std::vector<std::string>();
    int hopLimit = 5;
    LoraMessage message = LoraMessage(senders, 1, "test", rssi, hopLimit, 0, 0);

    std::string out = message.createPacket();

    std::string expected = "~$5$1c$$85$$37$1^test^1518f6dc~";
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), out.c_str());
}

void create_packet_from_loramessage_multiple_senders() {
    auto senders = std::vector<moduleAddress>{55, 133};
    auto hopLimit = 3;
    auto rssi = std::vector<std::string>{"RSSI37"};
    auto snr = -90;
    int currentRssi = -60;
    LoraMessage message = LoraMessage(senders, 1, "test", rssi, hopLimit, currentRssi, snr);

    std::string out = message.createPacket();

    std::string expected = "~$3$85$RSSI37$37$1^test^29280326~";
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), out.c_str());
}

void create_own_packet_from_loramessage_multiple_senders() {
    auto senders = std::vector<moduleAddress>{55, 133};
    auto hopLimit = 2;
    auto rssi = std::vector<std::string>{"RSSI37"};
    auto snr = -90;
    int currentRssi = 250;
    LoraMessage message = LoraMessage(senders, 1, "test", rssi, hopLimit, currentRssi, snr);

    std::string out = message.createPacketForSending();

    std::string expected = "~$2$1c$fa$85$RSSI37$37$1^test^ec600da9~";
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), out.c_str());
}

void create_generatedMessage() {
    auto senders = std::vector<moduleAddress>{55};
    auto rssi = std::vector<std::string>{};
    auto hopLimit = 100;
    auto message = GeneratedMessage(senders, 1, "test", rssi, hopLimit);

    std::string out = message.createPacket();

    std::string expected = "~$64$37$1^test^9e202503~";
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), out.c_str());
}

void decrement() {
    auto senders = std::vector<moduleAddress>{55};
    auto rssi = std::vector<std::string>{};
    auto hopLimit = 100;
    auto message = GeneratedMessage(senders, 1, "test", rssi, hopLimit);

    message.decrementHopLimit();
    std::string out = message.createPacket();

    std::string expected = "~$63$37$1^test^eab8cc8c~";
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), out.c_str());
}

void packet_for_sending_no_senders() {
    auto senders = std::vector<moduleAddress>{};
    auto rssi = std::vector<std::string>{};
    auto hopLimit = 100;
    auto message = GeneratedMessage(senders, 1, "test", rssi, hopLimit);

    message.decrementHopLimit();
    std::string out = message.createPacketForSending();

    std::string expected = "~$63$1c$1^test^c1f30ccb~";
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), out.c_str());
}

void decode_message_from_packet() {
    std::string packet = "~$37$1^test^40672562~";
    LoraMessage out = LoraMessage(packet, 0, 0);

    moduleAddress expectedSender = 55;
    moduleAddress expectedDestination = 1;
    std::string expectedMessage = "test";

    TEST_ASSERT_EQUAL(expectedSender, out.getOriginalSender());
    TEST_ASSERT_EQUAL(expectedDestination, out.getDestination());
    TEST_ASSERT_EQUAL_STRING(expectedMessage.c_str(), out.getContent().c_str());
}

void dont_allow_merged_packets() {
    std::string packet = "~$37$0^d^b7e09b90~~$37$0^d^b7e09b90~";
    TEST_ASSERT_FALSE(isPacketCorrect(packet));
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
    LoraMessage message = LoraMessage(packet, 0, 0);
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, message.getOriginalSender());
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, message.getDestination());
    TEST_ASSERT_EQUAL_STRING("", message.getContent().c_str());

    TEST_ASSERT_FALSE(isPacketCorrect(packet));
}

void no_crc_packet() {
    std::string packet = "~$1C$14$85$RSSI37$37$1^test^~";
    TEST_ASSERT_FALSE(isPacketCorrect(packet));
}

void only_crc() {
    std::string packet = "^~";
    LoraMessage message = LoraMessage(packet, 0, 0);
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, message.getOriginalSender());
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, message.getDestination());
    TEST_ASSERT_EQUAL_STRING("", message.getContent().c_str());

    TEST_ASSERT_FALSE(isPacketCorrect(packet));
}

void random_corruption() {
    std::string packet = "~$9Be&!*&$%!JSAFT@!%!&#HFA(T#@QUNst^40672562~";
    LoraMessage message = LoraMessage(packet, 0, 0);
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, message.getOriginalSender());
    TEST_ASSERT_EQUAL(INVALID_ADDRESS, message.getDestination());
    TEST_ASSERT_EQUAL_STRING("", message.getContent().c_str());

    TEST_ASSERT_FALSE(isPacketCorrect(packet));
}

void random_corruption_but_crc_is_correct() {
    std::string packet = "~$9Be&!*&$%!JSAFT@!%!&#HFA(T#@QUNst^0dab27ed~";
    LoraMessage message = LoraMessage(packet, 0, 0);
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
    LoraMessage message = LoraMessage(packet, 0, 0);

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

void stress_test() {
    std::string packet = "~$30$9B$RSSI21$21$RSSI37$37$1^test^450d3fed~~$30$9B$RSSI21$21$RSSI37$37$1^test^450d3fed~~$30$9B$RSSI21$21$RSSI37$37$1^test^450d3fed~~$30$9B$RSSI21$21$RSSI37$37$1^test^450d3fed~~$30$9B$RSSI21$21$RSSI37$37$1^test^450d3fed~~$30$9B$RSSI21$21$RSSI37$37$1^test^450d3fed~~$30$9B$RSSI21$21$RSSI37$37$1^test^450d3fed~~$30$9B$RSSI21$21$RSSI37$37$1^test^450d3fed~~$30$9B$RSSI21$21$RSSI37$37$1^test^450d3fed~~$30$9B$RSSI21$21$RSSI37$37$1^test^450d3fed~~$30$9B$RSSI21$21$RSSI37$37$1^test^450d3fed~";
    TEST_ASSERT_FALSE(isPacketCorrect(packet));
}

void big_crc_overflow() {
    std::string packet = "~$63$3$e0$37$0^{\"t\":\"p\",\"m\":\"[\\\"random prime number: 2137\\\"]\",\"c\":63}^f14e1278~";
    TEST_ASSERT_TRUE(isPacketCorrect(packet));
}

void setup() {
    UNITY_BEGIN();
    setupAddressHandler();
    RUN_TEST(create_packet_from_message);
    RUN_TEST(create_packet_from_message_multiple_senders);
    RUN_TEST(create_packet_from_loramessage_multiple_senders);
    RUN_TEST(create_own_packet_from_loramessage_multiple_senders);
    RUN_TEST(create_generatedMessage);
    RUN_TEST(decode_message_from_packet);
    RUN_TEST(decrement);
    RUN_TEST(dont_allow_merged_packets);
    RUN_TEST(packet_for_sending_no_senders);
    RUN_TEST(something_that_isnt_even_packet);
    RUN_TEST(only_crc);
    RUN_TEST(no_crc_packet);
    RUN_TEST(random_corruption);
    RUN_TEST(random_corruption_but_crc_is_correct);
    RUN_TEST(also_accept_hops);
    RUN_TEST(incorrect_nth_last_address);
    RUN_TEST(nth_last_address_check);
    RUN_TEST(nth_last_address_table_element_check);
    RUN_TEST(get_validated_part_correct);
    RUN_TEST(crc_correct);
    RUN_TEST(packet_correct);
    RUN_TEST(stress_test);
    RUN_TEST(big_crc_overflow);
    UNITY_END();
}

void loop() {
    // clean stuff up here
}
