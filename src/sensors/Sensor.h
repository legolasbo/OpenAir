#ifndef SENSOR_H
#define SENSOR_H

#include "Measurements.h"
#include "SensorTypes.h"
#include "ConnectionTypes.h"

class Sensor : public Measurements::Measurement {

    public:
        virtual SensorType getSensorType() {
            return SensorType::UNKNOWN_SENSOR_TYPE;
        }
        virtual ConnectionType getConnectionType() {
            return ConnectionType::UNKNOWN_CONNECTION_TYPE;
        }
        virtual Measurements::MeasurementTypeList getMeasurementTypes() {
            return Measurements::MeasurementTypeList {};
        }
        bool supportsMeasurementType(Measurements::Type type) {
            for (Measurements::Type t : this->getMeasurementTypes()) {
                if (t == type) {
                    return true;
                }
            }
            return false;
        }
};


#endif
