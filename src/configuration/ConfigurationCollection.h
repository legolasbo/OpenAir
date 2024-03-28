#ifndef CONFIGURATION_COLLECTION_H
#define CONFIGURATION_COLLECTION_H

#include <vector>
#include <map>
#include "GenericConfiguration.h"
#include "../DependencyInjectionContainer.hpp"

template <typename ConfigurationType>

class ConfigurationCollection {
    protected:
    DI * container;
    std::map<std::string, ConfigurationType*> configs;
    std::vector<std::string> uuids;

    void eraseUuid(std::string uuid) {
        std::vector<std::string> remaining;

        for (auto u : this->uuids)
        {
            if (u != uuid) {
                remaining.push_back(u);
            }
        }

        this->uuids = remaining;
    }

    public:
    ConfigurationCollection(DI * container) {
        this->container = container;
    }

    bool isDirty() {
        for (auto config : this->configs) {
            if (config.second->isDirty()) {
                return true;
            }
        }
        return false;
    }

    void markClean() {
        for (auto config : this->configs) {
            config.second->markClean();
        }
    }

    bool identicalConfigExists(ConfigurationType *config) {
        if (this->configs.size() == 0) {
            return false;
        }

        for (auto & c : this->configs) {
            if (c.second->equals(config)) {
                return true;
            }
        }
        return false;
    }

    void add(ConfigurationType * config) {
        if (config == nullptr) {
            return;
        }

        if (this->identicalConfigExists(config)) {
            return;
        }

        std::string uuid = config->getUuid();
        this->uuids.push_back(uuid);
        this->configs.insert(std::make_pair(uuid, config));
    }

    std::vector<std::string> getUuids() {
        return this->uuids;
    }

    ConfigurationType * get(std::string uuid) {
        try {
            return this->configs.at(uuid);
        } 
        catch(std::exception e) {
            Serial.printf("failed to get %s because %s\n", uuid.c_str(), e.what());
        }

        return nullptr;
    }

    void erase(std::string uuid) {
        ConfigurationType * item = this->get(uuid);
        this->configs.erase(uuid);
        this->eraseUuid(uuid);
        delete(item);
        Serial.printf("Deleted: %s\n", uuid.c_str());
    }

    JsonDocument toJson() {
        JsonDocument doc;

        for (std::string uuid : this->uuids) {
            doc[uuid] = this->configs.at(uuid)->toJson();
        }

        return doc;
    }

};

#endif
