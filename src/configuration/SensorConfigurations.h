#pragma once

#include "SensorConfiguration.h"
#include "ConfigurationCollection.h"
#include <vector>
#include <map>
#include <ArduinoJson.h>
#include "../DependencyInjectionContainer.hpp"

class SensorConfigurations : public ConfigurationCollection<SensorConfiguration> {
    private:

    public:
    SensorConfigurations(DI &container) : ConfigurationCollection<SensorConfiguration>(container) {}

    std::vector<SensorConfiguration *> getConfigurationsFor(SensorConnector connector) {
        std::vector<SensorConfiguration *> sensors;

        for (auto &pair : this->configs) {
            if(pair.second->getSensorConnector() == connector) {
                sensors.push_back(pair.second);
            }
        }

        return sensors;
    }

    static std::shared_ptr<SensorConfigurations> fromJson(DI &container, JsonObject sensors) {
        std::shared_ptr<SensorConfigurations> instance = std::make_shared<SensorConfigurations>(container);

        for (JsonPair p : sensors) {
            try
            {
                instance->add(SensorConfiguration::fromJson(container, p.value().as<JsonObject>()));
            }
            catch(const std::exception& e)
            {
                Log.errorln("Unable to add sensor from json because: %s", e.what());
            }
        }

        return instance;
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

    SensorConfiguration * create(SensorType t) {
        return new SensorConfiguration(this->container, t);
    }
};
