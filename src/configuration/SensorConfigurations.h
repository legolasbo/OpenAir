
#ifndef SENSOR_CONFIGURATIONS_H
#define SENSOR_CONFIGURATIONS_H

#include "../sensors/SensorTypes.h"
#include "../sensors/SensorFactory.h"
#include "SensorConfiguration.h"
#include <vector>
#include <ArduinoJson.h>

class SensorConfigurations {
    private:
    std::vector<SensorConfiguration> configs;

    public:

    SensorConfigurations() {
    }

    SensorConfigurations(std::vector<SensorConfiguration> configs) {
        this->configs = configs;
    }

    bool identicalConfigExists(SensorConfiguration &config) {
        if (this->configs.size() == 0) {
            return false;
        }

        for(SensorConfiguration c : this->configs) {
            if (c.equals(config)) {
                return true;
            }
        }
        return false;
    }

    void add(SensorConfiguration &config) {
        if (this->identicalConfigExists(config)) {
            throw std::invalid_argument("Unable to add configuration it already exists");
        }

        Serial.print("Inserting config: ");
        Serial.println(config.getMachineName());
        this->configs.push_back(config);
        return;
    }

    void add(SensorConfiguration *config) {
        this->add(*config);
    }

    std::vector<SensorConfiguration> getConfigurationsFor(SensorConnector connector) {
        Serial.print("Getting configurations for ");
        Serial.println(ToString(connector));
        std::vector<SensorConfiguration> sensors;

        for(SensorConfiguration config : this->configs) {
            Serial.print("Checking: ");
            Serial.println(config.getMachineName());

            if(config.getSensorConnector() == connector) {
                Serial.print("Selecting: ");
                Serial.println(config.getMachineName());
                sensors.push_back(config);
            }
        }

        return sensors;
    }

    std::vector<SensorConfiguration> all() {
        return this->configs;
    }

    SensorConfiguration *get(std::string machineName) {
        for (size_t i = 0; i < this->configs.size(); i++)
        {
            if (this->configs.at(i).getMachineName() == machineName) {
                return &this->configs.at(i);
            }
        }

        throw std::invalid_argument("Unknown machine name: " + machineName);        
    }

    JsonDocument toJson() {
        JsonDocument doc;

        for (size_t i = 0; i < this->configs.size(); i++)
        {
            doc[i] = this->configs.at(i).toJson();
        }

        return doc;
    }

    static SensorConfigurations * fromJson(JsonArray sensors) {
        if (sensors.size() == 0) {
            return new SensorConfigurations();
        }

        std::vector<SensorConfiguration> configs;
        for (int i = 0; i < sensors.size(); i++) {
            try
            {
                SensorConfiguration sens = SensorConfiguration::fromJson(sensors[i].as<JsonObject>());
                configs.push_back(sens);
            }
            catch(const std::exception& e)
            {
                Serial.println(e.what());
            }
        }
        return new SensorConfigurations(configs);
    }

    bool exists(const char * machineName) {
        for (size_t i = 0; i < this->configs.size(); i++) {
            if(strcmp(this->configs.at(i).getMachineName(), machineName) == 0) {
                return true;
            }
        }

        return false;
    }
};

#endif
