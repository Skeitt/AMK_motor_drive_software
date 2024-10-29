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

    TEST_ASSERT_EQUAL(0x00EB, actualValues2.tempMotor);
    TEST_ASSERT_EQUAL(0x00DE, actualValues2.tempInverter);
    TEST_ASSERT_EQUAL(0x0000, actualValues2.errorInfo);
    TEST_ASSERT_EQUAL(0x00E1, actualValues2.tempIGBT);
}

int runUnityTests(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_parse_valid_message);
    return UNITY_END();
}

int main(int argc, char **argv) {
    runUnityTests();
    return 0;
}
