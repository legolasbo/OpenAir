
#ifndef SENSOR_CONFIGURATIONS_H
#define SENSOR_CONFIGURATIONS_H

#include "../sensors/SensorTypes.h"
#include "../sensors/SensorFactory.h"
#include "SensorConfiguration.h"
#include <vector>

class SensorConfigurations {
    private:
    std::vector<SensorConfiguration> configs;

    public:

    SensorConfigurations() {

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
        Serial.println(config.getMachineName().c_str());
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
            Serial.println(config.getMachineName().c_str());

            if(config.getSensorConnector() == connector) {
                Serial.print("Selecting: ");
                Serial.println(config.getMachineName().c_str());
                sensors.push_back(config);
            }
        }

        return sensors;
    }

    std::vector<SensorConfiguration> all() {
        return this->configs;
    }

    SensorConfiguration get(std::string machineName) {
        for (size_t i = 0; i < this->configs.size(); i++)
        {
            if (this->configs.at(i).getMachineName() == machineName) {
                return this->configs.at(i);
            }
        }

        throw std::invalid_argument("Unknown machine name: " + machineName);        
    }
};

#endif
