#pragma once

#include <vector>
#include <string.h> 

enum CalculatorType {
    UNKNOWN_CALCULATOR_TYPE,
    HUMIDITY_CALCULATOR,
    SWITCH_POSITION_CALCULATOR,
};

const char* ToString(CalculatorType v) {
    switch (v)
    {
        case HUMIDITY_CALCULATOR:                return "Humidity";
        case SWITCH_POSITION_CALCULATOR:          return "Switch position";
        default:                            return "[Unknown sensor]";
    }
}

const char* ToMachineName(CalculatorType v) {
    switch (v)
    {
        case HUMIDITY_CALCULATOR:                return "humidity";
        case SWITCH_POSITION_CALCULATOR:          return "switch";
        default:                            return "unknown";
    }
}

CalculatorType CalculatorTypeFromMachineName(const char * name) {
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
        HUMIDITY_CALCULATOR,
        SWITCH_POSITION_CALCULATOR,
    };
}
