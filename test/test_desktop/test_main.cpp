#include <unity.h>
#include "test_parsing.hpp"
#include "test_diagnostic.hpp"

void setUp(void) { }

void tearDown(void) { }

int main(int argc, char **argv) {
    UNITY_BEGIN();

    runParsingTests();
    runDiagnosticTests();

    return UNITY_END();
}
