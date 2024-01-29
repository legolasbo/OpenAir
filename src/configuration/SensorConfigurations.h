
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
        for(SensorConfiguration c : this->configs) {
            if (c.equals(config)) {
                return true;
            }
        }
        return false;
    }

    void add(SensorConfiguration &config) {
        if (!this->identicalConfigExists(config)) {
            Serial.print("Inserting config: ");
            Serial.println(config.getMachineName());
            this->configs.push_back(config);
            return;
        }
        
        throw std::invalid_argument("Unable to add configuration it already exists");
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
};

#endif
