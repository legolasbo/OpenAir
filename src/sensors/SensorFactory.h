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
#include "../factories/Factory.hpp"

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

class SensorFactory : public Factory<Sensor> {
    private:
        I2CManager * i2cManager;
        SensorConfigurations * configs;

        I2CSensor* createI2CSensor(SensorConfiguration * config) {
            switch (config->getSensorType()) {
                case SHT20Sensor: return new SHT20Reader(config->getUuid(), this->i2cManager->fromConnector(config->getSensorConnector()));
                case ThreePositionSwitchSensor: return new ThreePositionSwitch(config->getUuid(), this->i2cManager->fromConnector(config->getSensorConnector()));
                default: throw std::invalid_argument("Unknown sensor type");
            }
        }

        Sensor* createSensorFromConfiguration(SensorConfiguration* config) {
            switch (config->getConnectionType()) {
                case I2C: return createI2CSensor(config);
                case UART: throw std::invalid_argument("Uart is not implemented yet");
                default: throw std::invalid_argument("unsupported connection type");
            }
        }

    public:
        SensorFactory(I2CManager *i2cManager, SensorConfigurations * config) {
            this->i2cManager = i2cManager;
            this->configs = config;
        }

        Sensor * fromUuid(std::string uuid) {
            Sensor * foundSensor = this->getInstance(uuid);
            if (foundSensor != nullptr) {
                return foundSensor;
            }

            if (!this->configs->exists(uuid)) {
                return nullptr;
            }

            SensorConfiguration * sensorConfig = this->configs->get(uuid);

            Sensor * createdSensor = this->createSensorFromConfiguration(sensorConfig);
            this->registerInstance(uuid, createdSensor);

            return createdSensor;
        }

        Sensor* fromConfiguration(SensorConfiguration* config) {
            return this->fromUuid(config->getUuid());
        }

        Measurements::MeasurementTypeList availableMeasurementTypes() {
            Measurements::MeasurementTypeList types;

            for (std::string uuid : this->configs->getUuids()) {
                auto instance = this->fromUuid(uuid);
                if (instance == nullptr) {
                    Serial.printf("Could not load %s\n", uuid);
                    continue;
                }

                auto mtypes = instance->getMeasurementTypes();
                types.insert(mtypes.begin(), mtypes.end());
            }

            return types;
        }

        std::set<Sensor*> getSensorsSupportingMeasurements(Measurements::MeasurementTypeList measurements) {
            std::set<Sensor *> sensors;

            for (std::string uuid : this->configs->getUuids()) {
                Sensor* instance = this->fromUuid(uuid);
                if (instance == nullptr) {
                    continue;
                }

                if (!instance->getMeasurementTypes().intersects(measurements)) {
                    continue;
                }

                sensors.insert(instance);
            }

            return sensors;
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