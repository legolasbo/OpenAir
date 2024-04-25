#pragma once

#include "ConfigurationCollection.h"
#include "CalculatorConfiguration.h"
#include "SensorConfigurations.h"
#include "HumidityCalculatorConfiguration.h"
#include "SwitchPositionCalculatorConfiguration.h"

class CalculatorConfigurations : public ConfigurationCollection<CalculatorConfiguration> {
    private:
        static void deSerializationFailed(JsonObject json, const char * reason) {
            Serial.println(reason);
            serializeJsonPretty(json, Serial);
            Serial.println();
        }

    public:
    CalculatorConfigurations(std::shared_ptr<DI> container) : ConfigurationCollection<CalculatorConfiguration>(container) {}

    CalculatorConfiguration * create(CalculatorType type) {
        switch (type) {
            case HUMIDITY_CALCULATOR: return new HumidityCalculatorConfiguration(this->container);
            case SWITCH_POSITION_CALCULATOR: return new SwitchPositionCalculatorConfiguration(this->container);
            default: return nullptr;
        }
    }

    JsonDocument availableCalculatorTypes() {
        JsonDocument doc;

        Measurements::MeasurementTypeList types = this->container->resolve<SensorFactory>()->availableMeasurementTypes();

        for (auto calculatorType : KnownCalculatorTypes()) {
            CalculatorConfiguration * conf = this->create(calculatorType);
            if (conf == nullptr) {
                continue;
            }

            if(conf->supportedMeasurementTypes().intersects(types)) {
                doc[ToMachineName(calculatorType)] = ToString(calculatorType);
            }

            delete(conf);
        }

        return doc;
    }

    static std::shared_ptr<CalculatorConfigurations> fromJson(std::shared_ptr<DI> container, JsonObject calculators) {
        std::shared_ptr<CalculatorConfigurations> instance = std::make_shared<CalculatorConfigurations>(container);
        
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

            config->configureFromJson(json);
            instance->add(config);
        }

        return instance;
    }
};
