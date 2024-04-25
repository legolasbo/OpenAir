#pragma once

#include "SensorConfigurations.h"
#include "CalculatorConfigurations.h"
#include <ArduinoJson.h>
#include "SPIFFS.h"
#include "../constants.h"
#include "../DependencyInjectionContainer.hpp"

class Configuration {
    private:
    std::shared_ptr<DI> container;
    std::shared_ptr<SensorConfigurations>sensors;
    std::shared_ptr<CalculatorConfigurations>calculators;

    public:
    Configuration(std::shared_ptr<DI> container) : container(container) {
        this->sensors = std::make_shared<SensorConfigurations>(container);
        this->calculators = std::make_shared<CalculatorConfigurations>(container);
    }
    Configuration(std::shared_ptr<DI> container, std::shared_ptr<SensorConfigurations>sensors, std::shared_ptr<CalculatorConfigurations> calculators) : container(container) {
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

    static std::shared_ptr<Configuration> load(std::shared_ptr<DI> container) {
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

    static std::shared_ptr<Configuration> fromFile(std::shared_ptr<DI> container, const char * name) {
        if (!SPIFFS.begin(true)) {
            Serial.println("SPIFFS MOUNT FAILED!");
            return std::make_shared<Configuration>(container);
        }

        File file = SPIFFS.open(name);
        if (!file || file.isDirectory()) {
            Serial.printf("Failed to open file: %s\n", name);
            return std::make_shared<Configuration>(container);
        }

        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, file);

        file.close();
        if (err != err.Ok) {
            Serial.printf("Failed to deserialize %s because of %d\n", name, err);
            return std::make_shared<Configuration>(container);
        }

        return fromJson(container, doc);
    }

    std::shared_ptr<SensorConfigurations> getSensors() {
        return this->sensors;
    }

    std::shared_ptr<CalculatorConfigurations> getCalculators() {
        return this->calculators;
    }

    static std::shared_ptr<Configuration> fromJson(std::shared_ptr<DI> container, JsonDocument &json) {
        JsonObject sensorsJson = json["sensors"].as<JsonObject>();
        JsonObject calculatorsJson = json["calculators"].as<JsonObject>();

        std::shared_ptr<SensorConfigurations> sensors = SensorConfigurations::fromJson(container, sensorsJson);
        std::shared_ptr<CalculatorConfigurations> calculators = CalculatorConfigurations::fromJson(container, calculatorsJson);

        return std::make_shared<Configuration>(container, sensors, calculators);
    }

    JsonDocument toJson() {
        JsonDocument doc;

        doc["sensors"] = this->sensors->toJson();
        doc["calculators"] = this->calculators->toJson();

        return doc;
    }

};

