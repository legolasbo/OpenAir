#ifndef SENSOR_H
#define SENSOR_H

#include "Measurements.h"
#include "SensorTypes.h"
#include "ConnectionTypes.h"

class Sensor : public Measurements::Measurement {
    private:
        std::string uuid;

    public:
        Sensor(std::string uuid) {
            this->uuid = uuid;
        }
        std::string getUuid() {
            return this->uuid;
        }
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
};


#endif
