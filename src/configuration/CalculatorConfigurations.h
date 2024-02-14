#ifndef CALCULATOR_CONFIGURATIONS_H
#define CALCULATOR_CONFIGURATIONS_H

#include "ConfigurationCollection.h"
#include "CalculatorConfiguration.h"
#include "SensorConfigurations.h"
#include "ThreePositionSwitchCalculatorConfiguration.h"
#include "SHT20CalculatorConfiguration.h"
#include "../speedCalculators/CalculatorTypes.h"

class CalculatorConfigurations : public ConfigurationCollection<CalculatorConfiguration> {
    private:
        SensorConfigurations * sensors;

        static void deSerializationFailed(JsonObject json, const char * reason) {
            Serial.println(reason);
            serializeJsonPretty(json, Serial);
            Serial.println();
        }

    public:
    CalculatorConfigurations(SensorConfigurations * sensors) {
        this->sensors = sensors;
    }

    void addNew(CalculatorType type) {
        switch (type) {
        case ThreePositionSwitchCalculator:
            return this->add(new ThreePositionSwitchCalculatorConfiguration());
        case SHT20Calculator:
            return this->add(new SHT20CalculatorConfiguration());
        default:
            break;
        }
    }

    static CalculatorConfigurations * fromJson(JsonArray calculators, SensorConfigurations * sensorConfig) {
        CalculatorConfigurations * instance = new CalculatorConfigurations(sensorConfig);
        
        for (size_t i = 0; i < calculators.size(); i++) {
            JsonObject json = calculators[i].as<JsonObject>();
            if (!json.containsKey("type")) {
                deSerializationFailed(json, "Missing type key in json: ");
                continue;
            }
            
            CalculatorType type = CalculatorTypeFromMachineName(json["type"].as<const char*>());
            CalculatorConfiguration * config;
            switch (type) {
                case ThreePositionSwitchCalculator:
                    config = ThreePositionSwitchCalculatorConfiguration::fromJson(json);
                    break;
                case SHT20Calculator:
                    config = SHT20CalculatorConfiguration::fromJson(json);
                    break;
            }

            if (config == nullptr) {
                deSerializationFailed(json, "Unable to deserialize: ");
                continue;
            }

            try {

                if (sensorConfig->exists(config->getSensorMachineName().c_str())) {
                    Serial.printf("Skipping calculator config for non-existing sensor: %s\n", config->getSensorMachineName());
                    delete sensorConfig;
                    continue;
                }
                instance->add(config);
            }
            catch (const std::exception& e) {
                Serial.println(e.what());
            }
        }

        return instance;
    }
};


#endif