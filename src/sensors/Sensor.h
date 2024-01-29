#ifndef SENSOR_H
#define SENSOR_H

#include "SensorTypes.h"
#include "ConnectionTypes.h"

class Sensor {

    public:

        virtual SensorType getSensorType();
        virtual ConnectionType getConnectionType();
};


#endif
