#ifndef SHT20_CALCULATOR_CONFIGURATION
#define SHT20_CALCULATOR_CONFIGURATION

#include "SensorBasedCalculatorConfiguration.h"
#include "../speedCalculators/CalculatorTypes.h"
#include <vector>
#include "../sensors/SensorTypes.h"

class SHT20CalculatorConfiguration : public SensorBasedCalculatorConfiguration {
    public:

    SHT20CalculatorConfiguration(SensorConfigurations * sensorConfigs) : SensorBasedCalculatorConfiguration(sensorConfigs){}

    SensorTypeList supportedSensorTypes() {
        return SensorTypeList {
            SHT20Sensor,
        };
    }

    virtual CalculatorType type() {
        return SHT20_CALCULATOR;
    }

    virtual bool hasOption(std::string name) {
        return SensorBasedCalculatorConfiguration::hasOption(name);
    }

    virtual bool setOption(std::string name, std::string value) {
        return SensorBasedCalculatorConfiguration::setOption(name, value);
    }

    virtual JsonDocument getConfigurationOptions() {
        return SensorBasedCalculatorConfiguration::getConfigurationOptions();
    }

    virtual JsonDocument toJson() {
        return SensorBasedCalculatorConfiguration::toJson();
    }

    virtual JsonDocument toDetails() {
        JsonDocument doc = SensorBasedCalculatorConfiguration::toDetails();

        return doc;
    }

};

#endif
