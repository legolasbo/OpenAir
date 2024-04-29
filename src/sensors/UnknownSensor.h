#pragma once

#include "Sensor.h"

class UnknownSensor : public Sensor {
    public:
        ConnectionType getConnectionType() {
            return ConnectionType::UNKNOWN_CONNECTION_TYPE;
        }
        SensorType getSensorType() {
            return SensorType::UNKNOWN_SENSOR_TYPE;
        }
    
        Measurements::MeasurementTypeList getMeasurementTypes() {
            return {};
        }

        std::unordered_map<std::string, std::shared_ptr<Option>> availableOptions() {
            return {};
        }

};

