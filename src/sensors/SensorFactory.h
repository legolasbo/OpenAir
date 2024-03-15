#ifndef SENSOR_FACTORY_H
#define SENSOR_FACTORY_H

#include <Arduino.h>
#include <Wire.h>
#include <vector>
#include <map>
#include "I2CSensor.h"
#include "SHT20Reader.h"
#include "ThreePositionSwitch.h"
#include "SensorConnectors.h"
#include "../configuration/SensorConfigurations.h"

class I2CManager {
    public:
        TwoWire * x4;
        TwoWire * x6;
        
        I2CManager() {
            this->x4 = &Wire;
            this->x6 = &Wire1;

            x4->begin(S1_SDA, S1_SCL);
            x6->begin(S2_SDA, S2_SCL);
        }

        TwoWire* fromConnector(SensorConnector connector) {
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
        SensorConfigurations * configs;
        std::map<std::string, Sensor*> sensors;

        void registerSensor(std::string uuid, Sensor* sensor) {
            if (this->getSensor(uuid) != nullptr) {
                return;
            }
            this->sensors.insert(std::pair<std::string, Sensor*>(uuid, sensor));
        }

        I2CSensor* createI2CSensor(SensorType type, TwoWire *i2cBus) {
            switch (type) {
                case SHT20Sensor: return new SHT20Reader(i2cBus);
                case ThreePositionSwitchSensor: return new ThreePositionSwitch(i2cBus);
                default: throw std::invalid_argument("Unknown sensor type");
            }
        }

        Sensor* createSensorFromConfiguration(SensorConfiguration* config) {
            switch (config->getConnectionType()) {
                case I2C: return createI2CSensor(config->getSensorType(), this->i2cManager->fromConnector(config->getSensorConnector()));
                case UART: throw std::invalid_argument("Uart is not implemented yet");
                default: throw std::invalid_argument("unsupported connection type");
            }
        }

        Sensor * getSensor(std::string uuid) {
            for (auto pair : this->sensors) {
                if (pair.first == uuid) {
                    return pair.second;
                }
            }

            return nullptr;
        }

    public:
        SensorFactory(I2CManager *i2cManager, SensorConfigurations * config) {
            this->i2cManager = i2cManager;
            this->configs = config;
        }

        Sensor * fromUuid(std::string uuid) {
            Sensor * foundSensor = this->getSensor(uuid);
            if (foundSensor != nullptr) {
                return foundSensor;
            }

            if (!this->configs->exists(uuid)) {
                return nullptr;
            }

            SensorConfiguration * sensorConfig = this->configs->get(uuid);

            Sensor * createdSensor = this->createSensorFromConfiguration(sensorConfig);
            this->registerSensor(uuid, createdSensor);

            return createdSensor;
        }

        Sensor* fromConfiguration(SensorConfiguration* config) {
            return this->fromUuid(config->getUuid());
        }

        static std::vector<SensorType> knownSensorTypes() {
            return std::vector<SensorType>{
                SHT20Sensor, 
                ThreePositionSwitchSensor
                };
        }

        static JsonDocument knownSensorTypesJson() {
            JsonDocument doc;

            for (auto t : knownSensorTypes()) {
                doc[ToMachineName(t)] = ToString(t);
            }

            return doc;
        }

};

#endif