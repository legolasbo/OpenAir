#ifndef CALCULATOR_CONFIGURATIONS_H
#define CALCULATOR_CONFIGURATIONS_H

#include "ConfigurationCollection.h"
#include "CalculatorConfiguration.h"
#include "SensorConfigurations.h"
#include "ThreePositionSwitchCalculatorConfiguration.h"
#include "SHT20CalculatorConfiguration.h"
#include "../speedCalculators/CalculatorTypes.h"
#include "../speedCalculators/CalculatorFactory.h"

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

    void setSensors(SensorConfigurations * sensors) {
        this->sensors = sensors;
    }

    CalculatorConfiguration * create(CalculatorType type) {
        switch (type) {
            case SHT20Calculator: return new SHT20CalculatorConfiguration(this->sensors);
            case ThreePositionSwitchCalculator: return new ThreePositionSwitchCalculatorConfiguration(this->sensors);
            default: return nullptr;
        }
    }

    JsonDocument availableCalculatorTypes() {
        JsonDocument doc;

        SensorTypeList sensorTypes = this->sensors->getConfiguredSensorTypes();
        for (auto calculatorType : CalculatorFactory::knownCalculatorTypes()) {
            CalculatorConfiguration * conf = this->create(calculatorType);

            if(conf->supportedSensorTypes().intersects(sensorTypes)) {
                doc[ToMachineName(calculatorType)] = ToString(calculatorType);
            }
        }

        return doc;
    }

    JsonDocument getCalculatorOptions(const char * type) {
        CalculatorConfiguration * conf = this->create(CalculatorTypeFromMachineName(type));

        JsonDocument options = conf->getConfigurationOptions();
        delete conf;

        return options;
    }

    static CalculatorConfigurations * fromJson(JsonObject calculators, SensorConfigurations * sensorConfig) {
        CalculatorConfigurations * instance = new CalculatorConfigurations(sensorConfig);
        
        for (JsonPair p : calculators) {
            JsonObject json = p.value().as<JsonObject>();
            if (!json.containsKey("type")) {
                deSerializationFailed(json, "Missing type key in json: ");
                continue;
            }
            
            CalculatorType type = CalculatorTypeFromMachineName(json["type"].as<const char*>());
            CalculatorConfiguration * config = instance->create(type);

            if (config == nullptr) {
                deSerializationFailed(json, "Unable to deserialize: ");
                continue;
            }

            for (JsonPair kv : json) {
                if (kv.key() == "uuid") {
                    config->setUuid(kv.value().as<const char *>());
                    continue;
                }

                if (kv.key() == "type") {
                    continue;
                }

                bool success = false;
                if (kv.value().is<const char *>()) {
                    Serial.printf("Setting %s to %s\n", kv.key().c_str(), kv.value().as<const char *>());
                    success = config->setOption(kv.key().c_str(), kv.value().as<const char *>());
                }
                if (kv.value().is<int>()) {
                    Serial.printf("Setting %s to %d\n", kv.key().c_str(), kv.value().as<int>());
                    success = config->setOption(kv.key().c_str(), kv.value().as<int>());
                }

                if (!success) {
                    Serial.printf("Failed to set %s", kv.key().c_str());
                }
            }

            instance->add(config);
        }

        return instance;
    }
};


#endif