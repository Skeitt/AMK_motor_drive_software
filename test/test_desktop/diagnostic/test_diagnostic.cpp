#include <unity.h>
#include "AMKDiagnostic.hpp"
#include "AMKUtils.hpp"

// test valid error code
void test_valid_error(void)
{
    uint16_t errorInfo = 257;

    TEST_ASSERT_EQUAL_STRING("Control Panel", getError(errorInfo).first);
    TEST_ASSERT_EQUAL_STRING("System diagnostics", getError(errorInfo).second);
}

// Test invalid error code
void test_invalid_error(void)
{
    uint16_t errorInfo = 0;

    TEST_ASSERT_EQUAL_STRING("Unknown error category", getError(errorInfo).first);
    TEST_ASSERT_EQUAL_STRING("Unknown class of error", getError(errorInfo).second);
}

void runDiagnosticTests(void)
{
    RUN_TEST(test_valid_error);
    RUN_TEST(test_invalid_error);
}
