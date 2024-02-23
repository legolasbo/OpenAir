
#ifndef SENSOR_CONFIGURATIONS_H
#define SENSOR_CONFIGURATIONS_H

#include "../sensors/SensorTypes.h"
#include "../sensors/SensorFactory.h"
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

    bool exists(const char * machineName) {
        for (auto & pair : this->configs) {
            if (strcmp(pair.second->machineName(), machineName) == 0) {
                return true;
            }
        }

        return false;
    }
};

#endif
