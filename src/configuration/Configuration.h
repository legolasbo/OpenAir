#pragma once

#include "SensorConfigurations.h"
#include "CalculatorConfigurations.h"
#include <ArduinoJson.h>
#include "SPIFFS.h"
#include "../constants.h"
#include "../DependencyInjectionContainer.hpp"

class Configuration {
    private:
    DI &container;
    SensorConfigurations *sensors;
    CalculatorConfigurations *calculators;

    public:
    Configuration(DI &container) : container(container) {
        this->sensors = new SensorConfigurations(container);
        this->calculators = new CalculatorConfigurations(container);
    }
    Configuration(DI &container, SensorConfigurations *sensors, CalculatorConfigurations* calculators) : container(container) {
        this->sensors = sensors;
        this->calculators = calculators;
    }

    bool isDirty() {
        return this->sensors->isDirty() || this->calculators->isDirty();
    }

    void markClean() {
        this->sensors->markClean();
        this->calculators->markClean();
    }

    static Configuration * load(DI &container) {
        return Configuration::fromFile(container, CONFIGURATION_FILE_PATH);
    }

    void save() {
        if (!SPIFFS.begin(true)) {
            Serial.println("SPIFFS MOUNT FAILED!");
            return;
        }

        File f = SPIFFS.open(CONFIGURATION_FILE_PATH, FILE_WRITE);
        if (!f || f.isDirectory()) {
            Serial.printf("Failed to open file: %s\n", CONFIGURATION_FILE_PATH);
            return;
        }

        serializeJson(this->toJson(), f);

        f.close();
    }

    static Configuration * fromFile(DI &container, const char * name) {
        if (!SPIFFS.begin(true)) {
            Serial.println("SPIFFS MOUNT FAILED!");
            return new Configuration(container);
        }

        File file = SPIFFS.open(name);
        if (!file || file.isDirectory()) {
            Serial.printf("Failed to open file: %s\n", name);
            return new Configuration(container);
        }

        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, file);

        file.close();
        if (err != err.Ok) {
            Serial.printf("Failed to deserialize %s because of %d\n", name, err);
            return new Configuration(container);
        }

        return fromJson(container, doc);
    }

    SensorConfigurations * getSensors() {
        return this->sensors;
    }

    CalculatorConfigurations * getCalculators() {
        return this->calculators;
    }

    static Configuration * fromJson(DI &container, JsonDocument &json) {
        JsonObject sensorsJson = json["sensors"].as<JsonObject>();
        JsonObject calculatorsJson = json["calculators"].as<JsonObject>();

        SensorConfigurations * sensors = SensorConfigurations::fromJson(container, sensorsJson);
        CalculatorConfigurations * calculators = CalculatorConfigurations::fromJson(container, calculatorsJson);

        return new Configuration(container, sensors, calculators);
    }

    JsonDocument toJson() {
        JsonDocument doc;

        doc["sensors"] = this->sensors->toJson();
        doc["calculators"] = this->calculators->toJson();

        return doc;
    }

};

