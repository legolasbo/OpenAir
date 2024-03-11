#ifndef CALCULATOR_FACTORY_H
#define CALCULATOR_FACTORY_H

#include "../Factory.h"
#include "../configuration/CalculatorConfiguration.h"
#include "SpeedCalculator.h"

class CalculatorFactory : Factory<SpeedCalculator, CalculatorConfiguration> {
    protected:
    static SpeedCalculator * createInstanceFromConfiguration(CalculatorConfiguration * conf) {

    }

    public:
    static std::vector<CalculatorType> knownCalculatorTypes() {
        return std::vector<CalculatorType> {
            SHT20Calculator,
            ThreePositionSwitchCalculator,
        };
    }

    static JsonDocument knownCalculatorTypesJson() {
        JsonDocument doc;

        for (auto t : knownCalculatorTypes()) {
            doc[ToMachineName(t)] = ToString(t);

        }

        return doc;
    }

};

#endif
