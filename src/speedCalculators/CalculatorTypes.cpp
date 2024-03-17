#include "CalculatorTypes.h"
#include <string.h> 

const char* ToString(CalculatorType v) {
    switch (v)
    {
        case THREE_POSITION_SWITCH_CALCULATOR:     return "Three position switch";
        case SHT20_CALCULATOR:                   return "SHT20 temperature/humidity sensor";
        case HUMIDITY_CALCULATOR:                return "Humidity";
        case SWITCH_POSITION_CALCULATOR:          return "Switch position";
        default:                            return "[Unknown sensor]";
    }
}

const char* ToMachineName(CalculatorType v) {
    switch (v)
    {
        case THREE_POSITION_SWITCH_CALCULATOR:     return "3possw";
        case SHT20_CALCULATOR:                   return "sht20";
        case HUMIDITY_CALCULATOR:                return "humidity";
        case SWITCH_POSITION_CALCULATOR:          return "switch";
        default:                            return "unknown";
    }
}

CalculatorType CalculatorTypeFromMachineName(const char * name) {
    if (strcmp(name, ToMachineName(THREE_POSITION_SWITCH_CALCULATOR)) == 0) {
        return THREE_POSITION_SWITCH_CALCULATOR;
    }
    if (strcmp(name, ToMachineName(SHT20_CALCULATOR)) == 0) {
        return SHT20_CALCULATOR;
    }
    if (strcmp(name, ToMachineName(HUMIDITY_CALCULATOR)) == 0) {
        return HUMIDITY_CALCULATOR;
    }
    if (strcmp(name, ToMachineName(SWITCH_POSITION_CALCULATOR)) == 0) {
        return SWITCH_POSITION_CALCULATOR;
    }

    return UNKNOWN_CALCULATOR_TYPE;
}

std::vector<CalculatorType> KnownCalculatorTypes() {
    return {
        THREE_POSITION_SWITCH_CALCULATOR,
        SHT20_CALCULATOR,
        HUMIDITY_CALCULATOR,
        SWITCH_POSITION_CALCULATOR,
    };
}
