#pragma once

#include <vector>
#include <string.h> 

enum CalculatorType {
    CO2_CALCULATOR,
    HUMIDITY_CALCULATOR,
    MAPPING_CALCULATOR,
    SWITCH_POSITION_CALCULATOR,
    UNKNOWN_CALCULATOR_TYPE,
};

std::vector<CalculatorType> KnownCalculatorTypes() {
    return {
        CO2_CALCULATOR,
        HUMIDITY_CALCULATOR,
        MAPPING_CALCULATOR,
        SWITCH_POSITION_CALCULATOR,
    };
}

const char* ToString(CalculatorType v) {
    switch (v)
    {
        case CO2_CALCULATOR:                return "Co2";
        case HUMIDITY_CALCULATOR:           return "Humidity";
        case MAPPING_CALCULATOR:            return "Mapping";
        case SWITCH_POSITION_CALCULATOR:    return "Switch position";
        default:                            return "[Unknown sensor]";
    }
}

const char* ToMachineName(CalculatorType v) {
    switch (v)
    {
        case CO2_CALCULATOR:                return "co2";
        case HUMIDITY_CALCULATOR:           return "humidity";
        case MAPPING_CALCULATOR:            return "mapping";
        case SWITCH_POSITION_CALCULATOR:    return "switch";
        default:                            return "unknown";
    }
}

CalculatorType CalculatorTypeFromMachineName(const std::string &name) {
    for(auto type : KnownCalculatorTypes()) {
        if (name == ToMachineName(type)) {
            return type;
        }
    }

    return UNKNOWN_CALCULATOR_TYPE;
}
