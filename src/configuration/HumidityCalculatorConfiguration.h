#ifndef HUMIDITY_CALCULATOR_CONFIGURATION_H
#define HUMIDITY_CALCULATOR_CONFIGURATION_H

#include "SensorBasedCalculatorConfiguration.h"
#include "../speedCalculators/CalculatorTypes.h"
#include "../speedCalculators/HumidityCalculator.h"
#include "../factories/SensorFactory.h"
#include <vector>
#include "../Measurements.h"

class HumidityCalculatorConfiguration : public SensorBasedCalculatorConfiguration {
    public:
    HumidityCalculatorConfiguration(DI* container) : SensorBasedCalculatorConfiguration(container) {}

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

    virtual SpeedCalculator * createInstance() {
        Sensor * sensor = this->container->resolve<SensorFactory>()->fromUuid(this->getSensorUuid());
        if (sensor == nullptr) {
            return nullptr;
        }

        if (auto m = sensor->toMeasurement<Measurements::Humidity>()) {
            return new HumidityCalculator(m);
        }
        return nullptr;
    }

};

#endif
