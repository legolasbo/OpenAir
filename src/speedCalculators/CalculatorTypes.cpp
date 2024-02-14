#include "CalculatorTypes.h"
#include <string.h> 

const char* ToString(CalculatorType v) {
    switch (v)
    {
        case ThreePositionSwitchCalculator:     return "Three position switch";
        case SHT20Calculator:                   return "SHT20 temperature/humidity sensor";
        default:                            return "[Unknown sensor]";
    }
}

const char* ToMachineName(CalculatorType v) {
    switch (v)
    {
        case ThreePositionSwitchCalculator:     return "3possw";
        case SHT20Calculator:                   return "sht20";
        default:                            return "unknown";
    }
}

CalculatorType CalculatorTypeFromMachineName(const char * name) {
    if (strcmp(name, ToMachineName(ThreePositionSwitchCalculator)) == 0) {
        return ThreePositionSwitchCalculator;
    }
    if (strcmp(name, ToMachineName(SHT20Calculator)) == 0) {
        return SHT20Calculator;
    }

    return UNKNOWN_CALCULATOR_TYPE;
}
