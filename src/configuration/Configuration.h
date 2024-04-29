#pragma once

#include <ArduinoJson.h>
#include "SPIFFS.h"
#include "../constants.h"
#include "../repositories/SensorRepository.hpp"
#include "../repositories/CalculatorRepository.hpp"

class Configuration {
    private:
        const char * sensorsKey = "sensors";
        const char * calculatorsKey = "calculators";

        template <typename Repo>
        void loadRepo(JsonDocument &json, const char * key) {
            if (!json.containsKey(key)) {
                Log.warningln("No %s configuration present", key);
                return;
            }
            DI::GetContainer()->resolve<Repo>()->loadJson(json[key]);
        }

    public:
        void load() {
            this->fromFile(CONFIGURATION_FILE_PATH);
        }

        void fromFile( const char * name) {
            if (!SPIFFS.begin(true)) {
                Log.errorln("SPIFFS MOUNT FAILED!");
                return;
            }

            File file = SPIFFS.open(name);
            if (!file || file.isDirectory()) {
                Log.errorln("Failed to open file: %s\n", name);
                return;
            }

            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, file);

            file.close();
            if (err != err.Ok) {
                Log.errorln("Failed to deserialize %s because of %d\n", name, err);
                return;
            }

            this->fromJson(doc);
        }

        void fromJson(JsonDocument &json) {
            this->loadRepo<SensorRepository>(json, sensorsKey);
            this->loadRepo<CalculatorRepository>(json, calculatorsKey);
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

            if (Log.getLevel() >= LOG_LEVEL_TRACE) {
                serializeJsonPretty(this->toJson(), Serial);
            }

            f.close();
        }

        JsonDocument toJson() {
            JsonDocument doc;

            doc[sensorsKey] = DI::GetContainer()->resolve<SensorRepository>()->toJson();
            doc[calculatorsKey] = DI::GetContainer()->resolve<CalculatorRepository>()->toJson();

            return doc;
        }

};

