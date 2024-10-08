#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <vector>
#include <map>
#include "../I2CManager.h"
#include "../sensors/SHT20Reader.h"
#include "../sensors/SCD4x.h"
#include "../sensors/MqttSensor.h"
#include "../sensors/ThreePositionSwitch.h"
#include "../sensors/UnknownSensor.h"
#include "Repository.hpp"

class SensorRepository : public Repository<Sensor> {
    private:

    public:

        std::shared_ptr<Sensor> create(SensorType type) {
            switch (type) {
                case MqttSensor: return std::make_shared<MQTTSensor>();
                case SHT20Sensor: return std::make_shared<SHT20Reader>();
                case SCD4xSensor: return std::make_shared<SCD4x>();
                case ThreePositionSwitchSensor: return std::make_shared<ThreePositionSwitch>();
                default: return std::make_shared<UnknownSensor>();
            }
        }

        std::shared_ptr<Sensor> create(const std::string &type) override {
            return this->create(SensorTypeFromMachineName(type));
        }

        Measurements::MeasurementTypeList availableMeasurementTypes() {
            Measurements::MeasurementTypeList types;

            for (auto itr : this->instances) {
                auto instance = itr.second;

                auto mtypes = instance->getMeasurementTypes();
                types.insert(mtypes.begin(), mtypes.end());
            }

            return types;
        }

        std::set<std::shared_ptr<Sensor>> getSensorsSupportingMeasurements(Measurements::MeasurementTypeList measurements) {
            std::set<std::shared_ptr<Sensor>> sensors;

            for (auto itr : this->instances) {
                auto instance = itr.second;
                if (!instance->getMeasurementTypes().intersects(measurements)) {
                    continue;
                }

                sensors.insert(instance);
            }

            return sensors;
        }

        void loop() {
            for(auto itr : this->instances) {
                itr.second->loop();
            }
        }

        static std::vector<SensorType> knownSensorTypes() {
            return std::vector<SensorType>{
                MqttSensor,
                SHT20Sensor, 
                SCD4xSensor, 
                ThreePositionSwitchSensor,
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
