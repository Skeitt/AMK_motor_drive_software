#ifndef DEBUG_FLAGS_HPP
#define DEBUG_FLAGS_HPP

#include <Arduino.h>

#define DEBUG_LEVEL_NONE    0
#define DEBUG_LEVEL_ERROR   1
#define DEBUG_LEVEL_STATE   2
#define DEBUG_LEVEL_VALUES  3

#define DEBUG_LEVEL DEBUG_LEVEL_STATE

#define DEBUG_PRINT(level, format, ...) \
    do { if (level <= DEBUG_LEVEL) Serial.printf(format, ##__VA_ARGS__); } while(0)

#endif // DEBUG_FLAGS_HPP
