
#ifndef GENERIC_CONFIGURATIONS_H
#define GENERIC_CONFIGURATIONS_H

#include <string>
#include <vector>
#include <ArduinoJson.h>
#include "UUID.h"

class GenericConfiguration {
    protected:
        std::string uuid;

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

    virtual std::string getUuid() {
        return this->uuid;
    }

    bool isValid() {
        return false;
    }

    virtual bool equals(GenericConfiguration * other) {
        return this->uuid == other->uuid;
    }

    virtual JsonDocument toJson() {
        JsonDocument doc;

        doc["uuid"] = this->uuid;

        return doc;
    }

    virtual std::string editForm() {
        return "Generic config<br>";
    }
};

#endif
