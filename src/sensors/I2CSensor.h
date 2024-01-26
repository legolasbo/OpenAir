#ifndef I2CSENSOR_H
#define I2CSENSOR_H

#include <Wire.h>
#include "SensorTypes.h"

class I2CSensor {
    protected:
        TwoWire *i2cBus;

    public:
        I2CSensor(TwoWire *bus) {
            i2cBus = bus;
        }

        virtual SensorType getType();
};

#endif
