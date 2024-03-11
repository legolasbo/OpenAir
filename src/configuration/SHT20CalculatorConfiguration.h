#ifndef SHT20_CALCULATOR_CONFIGURATION
#define SHT20_CALCULATOR_CONFIGURATION

#include "CalculatorConfiguration.h"
#include "../speedCalculators/CalculatorTypes.h"
#include <vector>
#include "../sensors/SensorTypes.h"

class SHT20CalculatorConfiguration : public CalculatorConfiguration {
    public:

    SHT20CalculatorConfiguration(SensorConfigurations * sensorConfigs) : CalculatorConfiguration(sensorConfigs){}

    SensorTypeList supportedSensorTypes() {
        return SensorTypeList {
            SHT20Sensor,
        };
    }

    virtual CalculatorType type() {
        return SHT20Calculator;
    }

    virtual JsonDocument toDetails() {
        JsonDocument doc = CalculatorConfiguration::toDetails();

        

        return doc;
    }

};

#endif
