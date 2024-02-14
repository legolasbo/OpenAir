#ifndef CALCULATOR_FACTORY_H
#define CALCULATOR_FACTORY_H

#include "../Factory.h"
#include "../configuration/CalculatorConfiguration.h"
#include "SpeedCalculator.h"

class CalculatorFactory : Factory<SpeedCalculator, CalculatorConfiguration> {
    protected:
    CalculatorConfiguration * createInstanceFromConfiguration(CalculatorConfiguration * conf) {

    }
};

#endif
