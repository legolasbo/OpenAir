#ifndef CONFIGURATION_COLLECTION_H
#define CONFIGURATION_COLLECTION_H

#include <vector>
#include <map>
#include "GenericConfiguration.h"

template <typename ConfigurationType>

class ConfigurationCollection {
    protected:
    std::map<std::string, ConfigurationType*> configs;
    std::vector<std::string> uuids;

    public:
    ConfigurationCollection() {}

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
        auto search = this->configs.find(uuid);
        if (search != this->configs.end()) {
            return search->second;
        }

        throw std::invalid_argument("Unknown uuid: " + std::string(uuid));
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
