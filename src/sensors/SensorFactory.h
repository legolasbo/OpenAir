#ifndef SENSOR_FACTORY_H
#define SENSOR_FACTORY_H

#include <Arduino.h>
#include <vector>
#include "I2CSensor.h"
#include "SHT20Sensor/SHT20Reader.h"
#include "SHT20Sensor/SHT20Configuration.h"
#include "ThreePositionSwitch/ThreePositionSwitch.h"
#include "ThreePositionSwitch/ThreePositionSwitchConfiguration.h"

class I2CManager {
    public:
        TwoWire *x4;
        TwoWire *x6;
        I2CManager(TwoWire &x4, TwoWire &x6) {
            this->x4 = &x4;
            this->x6 = &x6;

            this->x4->begin(S1_SDA, S1_SCL);
            this->x6->begin(S2_SDA, S2_SCL);
        }

        TwoWire * fromConnector(SensorConnector connector) {
            switch(connector) {
                case X4: return this->x4;
                case X6: return this->x6;
                default: return this->x4;
            }
        }
};

class SensorFactory {
    private:
        I2CManager * i2cManager;

    public:
        SensorFactory(I2CManager *i2cManager) {
            this->i2cManager = i2cManager;
        }

        I2CSensor* createI2CSensor(SensorType type, TwoWire *i2cBus) {
            switch (type) {
                case SHT20Sensor: return new SHT20Reader(i2cBus);
                case ThreePositionSwitchSensor: return new ThreePositionSwitch(i2cBus);
                default: throw std::invalid_argument("Unknown sensor type");
            }
        }

        Sensor* fromConfiguration(SensorConfiguration* config) {
            switch (config->getConnectionType()) {
                case I2C: return createI2CSensor(config->getSensorType(), this->i2cManager->fromConnector(config->getSensorConnector()));
                case UART: throw std::invalid_argument("Uart is not implemented yet");
                default: throw std::invalid_argument("unsupported connection type");
            }
        }

};

#endif