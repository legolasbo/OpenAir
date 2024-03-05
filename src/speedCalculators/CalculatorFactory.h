#ifndef CALCULATOR_FACTORY_H
#define CALCULATOR_FACTORY_H

#include "../Factory.h"
#include "../configuration/CalculatorConfiguration.h"
#include "SpeedCalculator.h"

class CalculatorFactory : Factory<SpeedCalculator, CalculatorConfiguration> {
    protected:
    SpeedCalculator * createInstanceFromConfiguration(CalculatorConfiguration * conf) {

    }

    public:
    static JsonDocument knownCalculatorTypesJson() {
        JsonDocument doc;

        doc[ToMachineName(SHT20Calculator)]["name"] = ToString(SHT20Calculator);
        doc[ToMachineName(ThreePositionSwitchCalculator)]["name"] = ToString(ThreePositionSwitchCalculator);

        return doc;
    }

};

#endif
