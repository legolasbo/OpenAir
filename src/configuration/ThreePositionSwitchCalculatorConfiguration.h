#ifndef THREE_POSITION_SWITCH_CALCULATOR_CONFIGURATION
#define THREE_POSITION_SWITCH_CALCULATOR_CONFIGURATION

#include "CalculatorConfiguration.h"
#include "../speedCalculators/CalculatorTypes.h"
#include <vector>
#include "../sensors/SensorTypes.h"

class ThreePositionSwitchCalculatorConfiguration : public CalculatorConfiguration {
    public:
    ThreePositionSwitchCalculatorConfiguration(SensorConfigurations * sensorConfigs) : CalculatorConfiguration(sensorConfigs) {}

    virtual SensorTypeList supportedSensorTypes() {
        return SensorTypeList {
            ThreePositionSwitchSensor,
        };
    }

    virtual CalculatorType type() {
        return ThreePositionSwitchCalculator;
    }

    virtual JsonDocument toDetails() {
        JsonDocument doc = CalculatorConfiguration::toDetails();

        

        return doc;
    }
};

#endif
