#ifndef I2CSENSOR_H
#define I2CSENSOR_H

#include <Wire.h>
#include "SensorTypes.h"
#include "Sensor.h"

class I2CSensor : public Sensor {
    protected:
        TwoWire *i2cBus;

    public:
        static const ConnectionType connectionType = I2C;

        ConnectionType getConnectionType() {
            return I2CSensor::connectionType;
        }

        I2CSensor(std::string uuid, TwoWire *bus) : Sensor(uuid) {
            this->i2cBus = bus;
        }

};

#endif
