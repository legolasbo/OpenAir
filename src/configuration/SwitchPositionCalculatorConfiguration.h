#pragma once

#include "SensorBasedCalculatorConfiguration.h"
#include "../speedCalculators/SwitchPositionCalculator.h"
#include "../factories/SensorFactory.h"
#include "../Measurements.h"

class SwitchPositionCalculatorConfiguration : public SensorBasedCalculatorConfiguration {
    public:
    SwitchPositionCalculatorConfiguration(DI* container) : SensorBasedCalculatorConfiguration(container) {}

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

    virtual SpeedCalculator * createInstance() {
        std::string sensorId = this->getOption("sensor").toStr();
        Sensor * sensor = this->container->resolve<SensorFactory>()->fromUuid(sensorId);
        if (sensor == nullptr) {
            Log.errorln("Unable to resolve sensor %s in calculator %s", sensorId.c_str(), this->getUuid().c_str());
            return nullptr;
        }

        if (auto m = sensor->toMeasurement<Measurements::SwitchPosition>()) {
            return new SwitchPositionCalculator(m);
        }
        return nullptr;
    }

};

