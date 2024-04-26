#pragma once

#include "Measurements.h"
#include "../enums/SensorTypes.h"
#include "../enums/ConnectionTypes.h"
#include "../options/Configurable.h"

class Sensor : public Configurable, public Measurements::Measurement {
    public:
        Sensor() : Configurable() {}
        Sensor(std::string uuid) : Configurable(uuid) {}

        virtual ~Sensor() = default;
        virtual SensorType getSensorType() = 0;
        virtual ConnectionType getConnectionType() = 0;
        virtual Measurements::MeasurementTypeList getMeasurementTypes() = 0;
        bool supportsMeasurementType(Measurements::Type type) {
            for (Measurements::Type t : this->getMeasurementTypes()) {
                if (t == type) {
                    return true;
                }
            }
            return false;
        }

        template <typename T>
        std::shared_ptr<T> toMeasurement() {
            std::shared_ptr<T> measurement = std::shared_ptr<T>(dynamic_cast<T*>(this));
            if (measurement == nullptr) {
                Log.errorln("Could not cast sensor to %s Actual type %s", typeid(T*).name(), typeid(*this).name());
                return nullptr;
            }
            return measurement;
        }
};

