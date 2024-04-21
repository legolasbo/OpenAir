#pragma once

#include "SensorBasedCalculatorConfiguration.h"
#include "../speedCalculators/HumidityCalculator.h"
#include "../factories/SensorFactory.h"
#include <vector>
#include "../Measurements.h"

class HumidityCalculatorConfiguration : public SensorBasedCalculatorConfiguration {
    public:
    HumidityCalculatorConfiguration(DI &container) : SensorBasedCalculatorConfiguration(container) {}

    Measurements::MeasurementTypeList supportedMeasurementTypes() {
        return Measurements::MeasurementTypeList {
            Measurements::Type::HumidityMeasurement,
        };
    }

    virtual CalculatorType type() {
        return HUMIDITY_CALCULATOR;
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
        if (!this->isConfiguredOption("sensor")) {
            return nullptr;
        }

        Sensor * sensor = this->container.resolve<SensorFactory>()->fromUuid(this->getOption("sensor").toStr());
        if (sensor == nullptr) {
            return nullptr;
        }

        if (auto m = sensor->toMeasurement<Measurements::Humidity>()) {
            return new HumidityCalculator(m);
        }
        return nullptr;
    }

};

