
#ifndef GENERIC_CONFIGURATIONS_H
#define GENERIC_CONFIGURATIONS_H

#include <string>
#include <vector>
#include <ArduinoJson.h>
#include "UUID.h"
#include "../DependencyInjectionContainer.hpp"
#include "../options/Configurable.h"

class GenericConfiguration: public Configurable {
    protected:
        DI * container;
        std::string uuid;
        bool dirty = true;

    void markDirty() {
        this->dirty = true;
    }

    public:
    GenericConfiguration(DI *container) {
        this->container = container;
        UUID uuid;
        uuid.seed(esp_random(), esp_random());
        uuid.setRandomMode();
        uuid.generate();
        this->uuid = std::string(uuid.toCharArray());
    }
    GenericConfiguration(DI * container, std::string uuid) {
        this->container = container;
        this->uuid = uuid;
    }

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

    virtual bool hasOption(std::string name) {
        return false;
    }

    virtual bool oldSetOption(std::string name, int value) {
        return false;
    }

    virtual bool oldSetOption(std::string name, std::string value) {
        Log.warningln("Old set option accessed with %s -> %s", name.c_str(), value.c_str());
        return false;
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

    virtual std::string getUuid() {
        return this->uuid;
    }

    virtual bool isValid() = 0;

    virtual bool equals(GenericConfiguration * other) {
        return this->uuid == other->uuid;
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
