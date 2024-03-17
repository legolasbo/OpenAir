#ifndef HUMIDITY_CALCULATOR_CONFIGURATION_H
#define HUMIDITY_CALCULATOR_CONFIGURATION_H

#include "SensorBasedCalculatorConfiguration.h"
#include "../speedCalculators/CalculatorTypes.h"
#include "../speedCalculators/HumidityCalculator.h"
#include "../sensors/SensorFactory.h"
#include <vector>
#include "../Measurements.h"

class HumidityCalculatorConfiguration : public SensorBasedCalculatorConfiguration {
    public:
    HumidityCalculatorConfiguration(SensorConfigurations * sensorConfigs) : SensorBasedCalculatorConfiguration(sensorConfigs){}

    Measurements::MeasurementTypeList supportedMeasurementTypes() {
        return Measurements::MeasurementTypeList {
            Measurements::Type::HumidityMeasurement,
        };
    }

    virtual CalculatorType type() {
        return HUMIDITY_CALCULATOR;
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

    virtual SpeedCalculator * createInstance(SensorFactory * sensorFactory) {
        return new HumidityCalculator(sensorFactory->fromUuid(this->uuid));
    }

};

#endif
