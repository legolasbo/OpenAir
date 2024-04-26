#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <vector>
#include <map>
#include "../I2CManager.h"
#include "../sensors/I2CSensor.h"
#include "../sensors/SHT20Reader.h"
#include "../sensors/ThreePositionSwitch.h"
#include "../sensors/UnknownSensor.h"
#include "../configuration/SensorConfigurations.h"
#include "Repository.hpp"

class SensorRepository : public Repository<Sensor> {
    private:

    public:

        std::shared_ptr<Sensor> create(SensorType type) {
            switch (type) {
                case SHT20Sensor: return std::make_shared<SHT20Reader>();
                case ThreePositionSwitchSensor: return std::make_shared<ThreePositionSwitch>();
                default: return std::make_shared<UnknownSensor>();
            }
        }

        std::shared_ptr<Sensor> create(std::string type) {
            return this->create(SensorTypeFromMachineName(type));
        }

        Measurements::MeasurementTypeList availableMeasurementTypes() {
            Measurements::MeasurementTypeList types;

            for (std::string uuid : DI::GetContainer()->resolve<SensorConfigurations>()->getUuids()) {
                auto instance = this->getInstance(uuid);
                if (instance == nullptr) {
                    Serial.printf("Could not load %s\n", uuid);
                    continue;
                }

                auto mtypes = instance->getMeasurementTypes();
                types.insert(mtypes.begin(), mtypes.end());
            }

            return types;
        }

        std::set<std::shared_ptr<Sensor>> getSensorsSupportingMeasurements(Measurements::MeasurementTypeList measurements) {
            std::set<std::shared_ptr<Sensor>> sensors;

            for (std::string uuid : DI::GetContainer()->resolve<SensorConfigurations>()->getUuids()) {
                std::shared_ptr<Sensor> instance = this->getInstance(uuid);
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
