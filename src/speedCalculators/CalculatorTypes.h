#ifndef CALCULATOR_TYPES_H
#define CALCULATOR_TYPES_H

enum CalculatorType {
    UNKNOWN_CALCULATOR_TYPE,
    ThreePositionSwitchCalculator,
    SHT20Calculator,
};

const char* ToString(CalculatorType v);

const char* ToMachineName(CalculatorType v);

CalculatorType CalculatorTypeFromMachineName(const char * name);

#endif