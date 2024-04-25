#pragma once

#include "SensorBasedCalculatorConfiguration.h"
#include "../calculators/SwitchPositionCalculator.h"
#include "../factories/SensorFactory.h"
#include "../Measurements.h"

class SwitchPositionCalculatorConfiguration : public SensorBasedCalculatorConfiguration {
    public:
    SwitchPositionCalculatorConfiguration(std::shared_ptr<DI> container) : SensorBasedCalculatorConfiguration(container) {}

    Measurements::MeasurementTypeList supportedMeasurementTypes() {
        return Measurements::MeasurementTypeList {
            Measurements::Type::SwitchPositionMeasurement,
        };
    }

    virtual CalculatorType type() {
        return SWITCH_POSITION_CALCULATOR;
    }

    virtual JsonDocument toDetails() {
        JsonDocument doc = SensorBasedCalculatorConfiguration::toDetails();

        return doc;
    }

    virtual std::shared_ptr<SpeedCalculator> createInstance() {
        std::string sensorId = this->getOption("sensor").toStr();
        std::shared_ptr<Sensor> sensor = this->container->resolve<SensorFactory>()->fromUuid(sensorId);
        if (sensor == nullptr) {
            Log.errorln("Unable to resolve sensor %s in calculator %s", sensorId.c_str(), this->getUuid().c_str());
            return nullptr;
        }

        if (auto m = sensor->toMeasurement<Measurements::SwitchPosition>()) {
            return std::make_shared<SwitchPositionCalculator>(m);
        }
        return nullptr;
    }

};

