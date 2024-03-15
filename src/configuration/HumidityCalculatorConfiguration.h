#ifndef SHT20_CALCULATOR_CONFIGURATION
#define SHT20_CALCULATOR_CONFIGURATION

#include "SensorBasedCalculatorConfiguration.h"
#include "../speedCalculators/CalculatorTypes.h"
#include <vector>
#include "../Measurements.h"

class HumidityCalculatorConfiguration : public SensorBasedCalculatorConfiguration {
    public:
    HumidityCalculatorConfiguration(SensorConfigurations * sensorConfigs, SensorFactory * sensorFactory) : SensorBasedCalculatorConfiguration(sensorConfigs, sensorFactory){}

    Measurements::MeasurementTypeList supportedMeasurementTypes() {
        return Measurements::MeasurementTypeList {
            Measurements::Type::HumidityMeasurement,
        };
    }

    virtual CalculatorType type() {
        return HumidityCalculator;
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
