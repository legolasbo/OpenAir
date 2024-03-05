#ifndef FACTORY_H
#define FACTORY_H

#include <map>

template <typename Type, typename ConfigType>

class Factory {
    protected:
    std::map<ConfigType*, Type*> instances;

    Type* createInstanceFromConfiguration(ConfigType * conf) {
        return nullptr;
    };

    public:
    Type* fromConfiguration(ConfigType * conf) {
        auto it = this->instances.find(conf);
        if (it != this->instances.end()) {
            Serial.printf("Found instance for configuration: %s\n", conf->getMachineName());
            return it->second;
        }

        Type* instance = this->createInstanceFromConfiguration(conf);
        this->instances.insert(std::pair<SensorConfiguration*, Sensor*>(conf, instance));
        return instance;
    }
};

#endif