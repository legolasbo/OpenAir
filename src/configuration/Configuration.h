#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "SensorConfigurations.h"
#include "CalculatorConfigurations.h"
#include <ArduinoJson.h>
#include "SPIFFS.h"
#include "../constants.h"

class Configuration {
    private:
    SensorConfigurations *sensors;
    CalculatorConfigurations *calculators;

    public:
    Configuration() {
        this->sensors = new SensorConfigurations();
        this->calculators = new CalculatorConfigurations(this->sensors);
    }
    Configuration(SensorConfigurations *sensors, CalculatorConfigurations* calculators) {
        this->sensors = sensors;
        this->calculators = calculators;
    }

    static Configuration * load() {
        return Configuration::fromFile(CONFIGURATION_FILE_PATH);
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

    static Configuration * fromFile(const char * name) {
        if (!SPIFFS.begin(true)) {
            Serial.println("SPIFFS MOUNT FAILED!");
            return new Configuration();
        }

        File file = SPIFFS.open(name);
        if (!file || file.isDirectory()) {
            Serial.printf("Failed to open file: %s\n", name);
            return new Configuration();
        }

        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, file);

        file.close();
        if (err != err.Ok) {
            Serial.printf("Failed to deserialize %s because of %d\n", name, err);
            return new Configuration();
        }

        return fromJson(doc);
    }

    SensorConfigurations * getSensors() {
        return this->sensors;
    }

    CalculatorConfigurations * getCalculators() {
        return this->calculators;
    }

    static Configuration * fromJson(JsonDocument &json) {
        JsonArray sensorsJson = json["sensors"].as<JsonArray>();
        JsonArray calculatorsJson = json["calculators"].as<JsonArray>();

        SensorConfigurations * sensors = SensorConfigurations::fromJson(sensorsJson);
        CalculatorConfigurations * calculators = CalculatorConfigurations::fromJson(calculatorsJson, sensors);

        return new Configuration(sensors, calculators);
    }

    JsonDocument toJson() {
        JsonDocument doc;

        doc["sensors"] = this->sensors->toJson();
        doc["calculators"] = this->calculators->toJson();

        return doc;
    }

};

#endif
