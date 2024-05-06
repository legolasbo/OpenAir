#pragma once

#include "Sensor.h"

class UnknownSensor : public Sensor {
    public:
        SensorType getSensorType() override {
            return SensorType::UNKNOWN_SENSOR_TYPE;
        }
    
        Measurements::MeasurementTypeList getMeasurementTypes() override {
            return {};
        }

        void loop() override {}

        Measurements::Measurement provide (Measurements::Type mt) override {
            return Measurements::Measurement();
        }

        std::unordered_map<std::string, std::shared_ptr<Option>> availableOptions() override {
            return {};
        }

        std::vector<ConnectionType> getSupportedConnectionTypes() override {
            return {};
        }

};

