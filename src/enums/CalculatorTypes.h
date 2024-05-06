#pragma once

#include <vector>
#include <string.h> 

enum CalculatorType {
    HUMIDITY_CALCULATOR,
    SWITCH_POSITION_CALCULATOR,
    UNKNOWN_CALCULATOR_TYPE,
};

const char* ToString(CalculatorType v) {
    switch (v)
    {
        case HUMIDITY_CALCULATOR:           return "Humidity";
        case SWITCH_POSITION_CALCULATOR:    return "Switch position";
        default:                            return "[Unknown sensor]";
    }
}

const char* ToMachineName(CalculatorType v) {
    switch (v)
    {
        case HUMIDITY_CALCULATOR:           return "humidity";
        case SWITCH_POSITION_CALCULATOR:    return "switch";
        default:                            return "unknown";
    }
}

CalculatorType CalculatorTypeFromMachineName(const std::string &name) {
    if (name == ToMachineName(HUMIDITY_CALCULATOR)) {
        return HUMIDITY_CALCULATOR;
    }
    if (name == ToMachineName(SWITCH_POSITION_CALCULATOR)) {
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
