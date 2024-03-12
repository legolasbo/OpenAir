#ifndef CALCULATOR_CONFIGURATION_H
#define CALCULATOR_CONFIGURATION_H

#include <string>
#include <vector>
#include "../sensors/SensorTypes.h"
#include "../speedCalculators/CalculatorTypes.h"
#include "GenericConfiguration.h"

class CalculatorConfiguration : public GenericConfiguration {
    protected:
        SensorConfigurations * sensorConfigs;

    public:
    CalculatorConfiguration(SensorConfigurations * sensorConfigs) : GenericConfiguration() {
        this->sensorConfigs = sensorConfigs;
    }

    virtual CalculatorType type() {
        return UNKNOWN_CALCULATOR_TYPE;
    }

    virtual bool dependsOn(std::string uuid) {
        return false;
    }

    virtual SensorTypeList supportedSensorTypes() {
        SensorTypeList types;
        return types;
    }

    virtual JsonDocument getConfigurationOptions() {
        JsonDocument doc;

        doc["name"]["type"] = "text";
        doc["name"]["label"] = "name";

        doc["uuid"]["type"] = "hidden";
        doc["type"]["type"] = "hidden";

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
