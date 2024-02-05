#ifndef SENSOR_H
#define SENSOR_H

#include "SensorTypes.h"
#include "ConnectionTypes.h"

class Sensor {

    public:
        virtual SensorType getSensorType() {
            return SensorType::UNKNOWN_SENSOR_TYPE;
        }
        virtual ConnectionType getConnectionType() {
            return ConnectionType::UNKNOWN_CONNECTION_TYPE;
        }
};


#endif
