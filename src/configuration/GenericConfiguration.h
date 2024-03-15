
#ifndef GENERIC_CONFIGURATIONS_H
#define GENERIC_CONFIGURATIONS_H

#include <string>
#include <vector>
#include <ArduinoJson.h>
#include "UUID.h"

class GenericConfiguration {
    protected:
        std::string uuid;
        std::string name;

    public:
    GenericConfiguration() {
        UUID uuid;
        uuid.seed(esp_random(), esp_random());
        uuid.setRandomMode();
        uuid.generate();
        this->uuid = std::string(uuid.toCharArray());
    }
    GenericConfiguration(std::string uuid) {
        this->uuid = uuid;
    }

    virtual bool hasOption(std::string name) {
        return name == "name";
    }

    virtual bool setOption(std::string name, int value) {
        return false;
    }

    virtual bool setOption(std::string name, std::string value) {
        if (name == "name") {
            return this->setName(value);
        }

        return false;
    }

    virtual bool dependsOn(std::string uuid) {
        return false;
    }

    std::string getName() {
        return this->name;
    }

    bool setName(std::string name) {
        this->name = name;
        return true;
    }

    virtual void setUuid(std::string uuid) {
        this->uuid = uuid;
    }

    virtual std::string getUuid() {
        return this->uuid;
    }

    virtual bool isValid() {
        return false;
    }

    virtual bool equals(GenericConfiguration * other) {
        return this->uuid == other->uuid;
    }

    virtual JsonDocument toJson() {
        JsonDocument doc;

        doc["uuid"] = this->uuid;
        doc["name"] = this->name;

        return doc;
    }
};

#endif
