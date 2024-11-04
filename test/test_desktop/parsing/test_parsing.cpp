#include <unity.h>
#include "test_parsing.hpp"
#include "AMKUtils.hpp"

// Test parsing of real case message
void test_parse_valid_message(void)
{
    uint8_t data[8] = {0xEB, 0x00, 0xDE, 0x00, 0x00, 0x00, 0xE1, 0x00};
    ActualValues2 actualValues2 = parseActualValues2(data);

    TEST_ASSERT_EQUAL(235, actualValues2.tempMotor);
    TEST_ASSERT_EQUAL(222, actualValues2.tempInverter);
    TEST_ASSERT_EQUAL(0, actualValues2.errorInfo);
    TEST_ASSERT_EQUAL(225, actualValues2.tempIGBT);
}

void test_parse_valid_setpoints(void)
{
    Setpoints1 setpoints1 = {0x0000, 0x0000, 0x012C, 0x0000};
    CANMessage canMsg = parseSetpoints1(setpoints1, 0x0000);

    TEST_ASSERT_EQUAL(387, canMsg.getCanId());
    TEST_ASSERT_EQUAL(0x00, canMsg.m_data[0]);
    TEST_ASSERT_EQUAL(0x00, canMsg.m_data[1]);
    TEST_ASSERT_EQUAL(0x00, canMsg.m_data[2]);
    TEST_ASSERT_EQUAL(0x00, canMsg.m_data[3]);
    TEST_ASSERT_EQUAL(0x2C, canMsg.m_data[4]);
    TEST_ASSERT_EQUAL(0x01, canMsg.m_data[5]);
    TEST_ASSERT_EQUAL(0x00, canMsg.m_data[6]);
    TEST_ASSERT_EQUAL(0x00, canMsg.m_data[7]);
}

// Test parsing of zero values
void test_parse_zero_values(void)
{
    uint8_t data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    ActualValues2 actualValues2 = parseActualValues2(data);

    TEST_ASSERT_EQUAL(0, actualValues2.tempMotor);
    TEST_ASSERT_EQUAL(0, actualValues2.tempInverter);
    TEST_ASSERT_EQUAL(0, actualValues2.errorInfo);
    TEST_ASSERT_EQUAL(0, actualValues2.tempIGBT);
}

// Test parsing of mixed values (positive and negative, max and min)
void test_parse_mixed_values(void)
{
    uint8_t data[8] = {0xFF, 0x7F, 0x00, 0x80, 0xFF, 0xFF, 0xCD, 0xEF};
    ActualValues2 actualValues2 = parseActualValues2(data);

    TEST_ASSERT_EQUAL(32767, actualValues2.tempMotor);
    TEST_ASSERT_EQUAL(-32768, actualValues2.tempInverter);
    TEST_ASSERT_EQUAL(65535, actualValues2.errorInfo);
    TEST_ASSERT_EQUAL(-4147, actualValues2.tempIGBT);
}


void runParsingTests(void)
{
    RUN_TEST(test_parse_valid_message);
    RUN_TEST(test_parse_zero_values);
    RUN_TEST(test_parse_mixed_values);
    RUN_TEST(test_parse_valid_setpoints);
}
