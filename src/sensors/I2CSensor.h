#pragma once

#include <Wire.h>
#include "Sensor.h"

class I2CSensor : public Sensor {
    protected:
        TwoWire &i2cBus;

    public:
        static const ConnectionType connectionType = I2C;
        virtual ~I2CSensor() = default;

        ConnectionType getConnectionType() {
            return I2CSensor::connectionType;
        }

        I2CSensor(std::string uuid, TwoWire &bus) : Sensor(uuid), i2cBus(bus) {}

};

