#ifndef SENSOR_FACTORY_H
#define SENSOR_FACTORY_H

#include <Arduino.h>
#include <vector>
#include "I2CSensor.h"
#include "SHT20Reader.h"
#include "ThreePositionSwitch.h"
#include "SensorTypes.h"

enum ConnectionType {
    I2C,
    UART,
};

struct SensorDefinition {
    ConnectionType connection;
    SensorType type;
    String name;
};

class SensorFactory {
    private:
        std::vector<SensorDefinition> knownDefinitions;

    public:
        SensorFactory() {
            knownDefinitions.push_back({I2C, ThreePositionSwitchSensor, "Three-position switch"});
            knownDefinitions.push_back({I2C, SHT20Sensor, "SHT20 temperature and humidity sensor"});
        }

        I2CSensor* createI2CSensor(SensorType type, TwoWire *i2cBus) {
            switch (type) {
                case SHT20Sensor: return new SHT20Reader(i2cBus);
                case ThreePositionSwitchSensor: return new ThreePositionSwitch(i2cBus);
            }

        }
};


#endif