#pragma once

#include "SensorBasedCalculatorConfiguration.h"
#include "../calculators/HumidityCalculator.h"
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

    virtual std::shared_ptr<SpeedCalculator> createInstance() {
        if (!this->isConfiguredOption("sensor")) {
            return nullptr;
        }

        std::shared_ptr<Sensor> sensor = this->container.resolve<SensorFactory>()->fromUuid(this->getOption("sensor").toStr());
        if (sensor == nullptr) {
            return nullptr;
        }

        if (auto m = sensor->toMeasurement<Measurements::Humidity>()) {
            return std::make_shared<HumidityCalculator>(m);
        }
        return nullptr;
    }

};

