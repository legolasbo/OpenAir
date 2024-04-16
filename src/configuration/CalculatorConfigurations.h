#ifndef CALCULATOR_CONFIGURATIONS_H
#define CALCULATOR_CONFIGURATIONS_H

#include "ConfigurationCollection.h"
#include "CalculatorConfiguration.h"
#include "SensorConfigurations.h"
#include "HumidityCalculatorConfiguration.h"
#include "SwitchPositionCalculatorConfiguration.h"
#include "../speedCalculators/CalculatorTypes.h"

class CalculatorConfigurations : public ConfigurationCollection<CalculatorConfiguration> {
    private:
        static void deSerializationFailed(JsonObject json, const char * reason) {
            Serial.println(reason);
            serializeJsonPretty(json, Serial);
            Serial.println();
        }

    public:
    CalculatorConfigurations(DI * container) : ConfigurationCollection<CalculatorConfiguration>(container) {}

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

    static CalculatorConfigurations * fromJson(DI * container, JsonObject calculators) {
        CalculatorConfigurations * instance = new CalculatorConfigurations(container);
        
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


#endif