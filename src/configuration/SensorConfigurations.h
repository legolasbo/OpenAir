
#ifndef SENSOR_CONFIGURATIONS_H
#define SENSOR_CONFIGURATIONS_H

#include "../sensors/SensorTypes.h"
#include "SensorConfiguration.h"
#include "ConfigurationCollection.h"
#include <vector>
#include <map>
#include <ArduinoJson.h>

class SensorConfigurations : public ConfigurationCollection<SensorConfiguration> {
    private:

    public:

    std::vector<SensorConfiguration *> getConfigurationsFor(SensorConnector connector) {
        Serial.print("Getting configurations for ");
        Serial.println(ToString(connector));
        std::vector<SensorConfiguration *> sensors;

        for (auto &pair : this->configs) {
            Serial.print("Checking: ");
            Serial.println(pair.second->machineName());

            if(pair.second->getSensorConnector() == connector) {
                Serial.print("Selecting: ");
                Serial.println(pair.second->machineName());
                sensors.push_back(pair.second);
            }
        }

        return sensors;
    }

    static SensorConfigurations * fromJson(JsonObject sensors) {
        SensorConfigurations * instance = new SensorConfigurations();

        for (JsonPair p : sensors) {
            try
            {
                instance->add(SensorConfiguration::fromJson(p.value().as<JsonObject>()));
            }
            catch(const std::exception& e)
            {
                Serial.println(e.what());
            }
        }

        return instance;
    }

    bool exists(std::string uuid) {
        return this->get(uuid) != nullptr;
    }

    SensorTypeList getConfiguredSensorTypes() {
        SensorTypeList types;

        for (auto config : this->configs) {
            SensorType t = config.second->getSensorType();
            if (!types.includes(t)) {
                types.push_back(t);
            }
        }

        return types;
    }

    std::map<std::string, SensorType> getUuidsForTypes(std::vector<SensorType> types) {
        std::map<std::string, SensorType> uuids;

        for (auto uuid : this->uuids) {
            SensorType t = this->get(uuid)->getSensorType();

            for (SensorType targetType : types) {
                if (t == targetType) {
                    uuids.insert(std::pair<std::string, SensorType>(uuid, t));
                }   
            }
        }

        return uuids;
    }
};

#endif
