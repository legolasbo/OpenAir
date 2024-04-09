#ifndef SENSOR_FACTORY_H
#define SENSOR_FACTORY_H

#include <Arduino.h>
#include <Wire.h>
#include <vector>
#include <map>
#include "../I2CManager.h"
#include "../sensors/I2CSensor.h"
#include "../sensors/SHT20Reader.h"
#include "../sensors/ThreePositionSwitch.h"
#include "../sensors/SensorConnectors.h"
#include "../configuration/SensorConfigurations.h"
#include "Factory.hpp"

class SensorFactory : public Factory<Sensor> {
    private:

        I2CSensor* createI2CSensor(SensorConfiguration * config) {
            auto i2cManager = this->container->resolve<I2CManager>();
            auto connector = i2cManager->fromConnector(config->getSensorConnector());

            switch (config->getSensorType()) {
                case SHT20Sensor: return new SHT20Reader(config->getUuid(), connector);
                case ThreePositionSwitchSensor: return new ThreePositionSwitch(config->getUuid(), connector);
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
        SensorFactory(DI* container) : Factory<Sensor>(container) {}

        Sensor * fromUuid(std::string uuid) {
            Sensor * foundSensor = this->getInstance(uuid);
            if (foundSensor != nullptr) {
                return foundSensor;
            }

            if (!this->container->resolve<SensorConfigurations>()->exists(uuid)) {
                return nullptr;
            }

            SensorConfiguration * sensorConfig = this->container->resolve<SensorConfigurations>()->get(uuid);

            Sensor * createdSensor = this->createSensorFromConfiguration(sensorConfig);
            this->registerInstance(uuid, createdSensor);

            return createdSensor;
        }

        Sensor* fromConfiguration(SensorConfiguration* config) {
            return this->fromUuid(config->getUuid());
        }

        Measurements::MeasurementTypeList availableMeasurementTypes() {
            Measurements::MeasurementTypeList types;

            for (std::string uuid : this->container->resolve<SensorConfigurations>()->getUuids()) {
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

            for (std::string uuid : this->container->resolve<SensorConfigurations>()->getUuids()) {
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