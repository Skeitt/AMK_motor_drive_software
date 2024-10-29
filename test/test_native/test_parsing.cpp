#include <unity.h>
#include "AMKUtils.hpp"

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

void test_parse_valid_message()
{
    uint8_t data[8] = {0xEB, 0x00, 0xDE, 0x00, 0x00, 0x00, 0xE1, 0x00};
    ActualValues2 actualValues2 = parseActualValues2(data);

    TEST_ASSERT_EQUAL(235, actualValues2.tempMotor);
    TEST_ASSERT_EQUAL(222, actualValues2.tempInverter);
    TEST_ASSERT_EQUAL(0, actualValues2.errorInfo);
    TEST_ASSERT_EQUAL(225, actualValues2.tempIGBT);
}

void test_parse_zero_values()
{
    uint8_t data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    ActualValues2 actualValues2 = parseActualValues2(data);

    TEST_ASSERT_EQUAL(0, actualValues2.tempMotor);
    TEST_ASSERT_EQUAL(0, actualValues2.tempInverter);
    TEST_ASSERT_EQUAL(0, actualValues2.errorInfo);
    TEST_ASSERT_EQUAL(0, actualValues2.tempIGBT);
}

void test_parse_mixed_values()
{
    uint8_t data[8] = {0xFF, 0x7F, 0x00, 0x80, 0xFF, 0xFF, 0xCD, 0xEF};
    ActualValues2 actualValues2 = parseActualValues2(data);

    TEST_ASSERT_EQUAL(32767, actualValues2.tempMotor);
    TEST_ASSERT_EQUAL(-32768, actualValues2.tempInverter);
    TEST_ASSERT_EQUAL(65535, actualValues2.errorInfo);
    TEST_ASSERT_EQUAL(-4147, actualValues2.tempIGBT);
}


int runUnityTests(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_parse_valid_message);
    RUN_TEST(test_parse_zero_values);
    RUN_TEST(test_parse_mixed_values);
    return UNITY_END();
}

int main(int argc, char **argv) {
    runUnityTests();
    return 0;
}
