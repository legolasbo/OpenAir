#ifndef CALCULATOR_CONFIGURATION_H
#define CALCULATOR_CONFIGURATION_H

#include <string>
#include <vector>
#include "../sensors/SensorTypes.h"
#include "../speedCalculators/CalculatorTypes.h"
#include "GenericConfiguration.h"

class CalculatorConfiguration : public GenericConfiguration {
    private:
        SensorConfigurations * sensorConfigs;

    public:
    CalculatorConfiguration(SensorConfigurations * sensorConfigs) : GenericConfiguration() {
        this->sensorConfigs = sensorConfigs;
    }

    virtual CalculatorType type() {
        return UNKNOWN_CALCULATOR_TYPE;
    }

    virtual SensorTypeList supportedSensorTypes() {
        SensorTypeList types;
        return types;
    }

    virtual JsonDocument getConfigurationOptions(SensorConfigurations * sensors) {
        JsonDocument doc;

        doc["name"]["type"] = "text";
        doc["name"]["label"] = "name";

        doc["uuid"]["type"] = "hidden";
        doc["type"]["type"] = "hidden";

        if (this->supportedSensorTypes().size() > 0) {
            doc["sensor"]["type"] = "select";
            doc["sensor"]["label"] = "Sensor";

            std::map<std::string, SensorType> uuidMap = sensors->getUuidsForTypes(supportedSensorTypes());

            for (auto entry : uuidMap) {
                std::string name = sensors->get(entry.first)->getName();
                if (name == "") {
                    name.append("Unnamed ");
                    name.append(ToString(entry.second));
                }

                doc["sensor"]["options"][entry.first] = name;
            }
        }

        return doc;
    }

    virtual JsonDocument toJson() {
        JsonDocument doc = GenericConfiguration::toJson();

        doc["type"] = ToMachineName(this->type());

        return doc;
    }

    virtual JsonDocument toDetails() {
        JsonDocument doc;

        doc["uuid"]["label"] = "Uuid";
        doc["uuid"]["value"] = this->getUuid();
        doc["uuid"]["type"] = "key/value";

        doc["name"]["label"] = "Name";
        doc["name"]["value"] = this->getName();
        doc["name"]["type"] = "key/value";

        doc["type"]["label"] = "Calculator type";
        doc["type"]["value"] = ToString(this->type());
        doc["type"]["type"] = "key/value";

        return doc;
    }
};

#endif
