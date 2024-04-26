
#ifndef GENERIC_CONFIGURATIONS_H
#define GENERIC_CONFIGURATIONS_H

#include <string>
#include <vector>
#include <ArduinoJson.h>
#include "UUID.h"
#include "../DependencyInjectionContainer.hpp"
#include "../options/Configurable.h"

class GenericConfiguration: public Configurable {
    private:

    protected:
        std::string uuid;
        bool dirty = true;

    void markDirty() {
        this->dirty = true;
    }

    public:
    GenericConfiguration() : Configurable() {}
    GenericConfiguration(std::string uuid) : Configurable(uuid) {}

    virtual std::unordered_map<std::string, Option> availableOptions() {
        return {
            {"name", Option("Unnamed")}
        };
    };

    bool isDirty() {
        return this->dirty;
    }

    void markClean() {
        this->dirty = false;
    }

    virtual bool dependsOn(std::string uuid) {
        return false;
    }

    virtual std::string getName() {
        return this->getOption("name").toStr();
    }

    bool setName(std::string name) {
        return this->setOption("name", Option(name));
        this->markDirty();
        return true;
    }

    virtual void setUuid(std::string uuid) {
        this->uuid = uuid;
        this->markDirty();
    }

    virtual bool isValid() = 0;

    virtual bool equals(GenericConfiguration * other) {
        return this->uuid == other->uuid;
    }

    virtual JsonDocument toDetails() {
        JsonDocument doc = Configurable::toDetails();

        doc["uuid"]["label"] = "Uuid";
        doc["uuid"]["value"] = this->getUuid();
        doc["uuid"]["type"] = "key/value";

        return doc;
    }

    virtual JsonDocument toJson() {
        JsonDocument doc = Configurable::toJson();

        doc["uuid"] = this->uuid;

        return doc;
    }

    virtual bool configureFromJson(JsonObject doc) {
        if (!doc.containsKey("uuid")) {
            Log.errorln("Unable to configure %s without a uuid", typeid(*this).name());
        }
        
        this->uuid = doc["uuid"].as<std::string>();

        return Configurable::configureFromJson(doc);
    }

};

#endif
