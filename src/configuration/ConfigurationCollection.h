#ifndef CONFIGURATION_COLLECTION_H
#define CONFIGURATION_COLLECTION_H

#include <vector>
#include "GenericConfiguration.h"

template <typename ConfigurationType>

class ConfigurationCollection {
    protected:
    std::vector<ConfigurationType*> configs;

    public:
    ConfigurationCollection() {}
    ConfigurationCollection(std::vector<ConfigurationType*> configs) {
        this->configs = configs;
    }

    bool identicalConfigExists(ConfigurationType *config) {
        if (this->configs.size() == 0) {
            return false;
        }

        for (ConfigurationType * c : this->configs) {
            if (c->equals(config)) {
                return true;
            }
        }
        return false;
    }

    void add(ConfigurationType * config) {
        if (this->identicalConfigExists(config)) {
            return;
        }

        Serial.printf("Inserting config: %s\n", config->getUuid());
        this->configs.push_back(config);
    }

    std::vector<ConfigurationType *> all() {
        return this->configs;
    }

    ConfigurationType * get(char * machineName) {
        for (size_t i = 0; i < this->configs.size(); i++) {
            if (strcmp(this->configs.at(i)->machineName(), machineName) == 0) {
                return this->configs.at(i);
            }
        }


        throw std::invalid_argument("Unknown machine name: " + std::string(machineName));
    }

    JsonDocument toJson() {
        JsonDocument doc;

        Serial.printf("Serializing %d calculators\n", this->configs.size());

        for (size_t i = 0; i < this->configs.size(); i++) {
            Serial.printf("Serializing %s\n", this->configs.at(i)->getUuid().c_str());
            doc[i] = this->configs.at(i)->toJson();
        }

        return doc;
    }

};

#endif
