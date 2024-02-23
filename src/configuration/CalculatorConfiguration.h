#ifndef CALCULATOR_CONFIGURATION_H
#define CALCULATOR_CONFIGURATION_H

#include <string>
#include <vector>
#include "../sensors/SensorTypes.h"
#include "../speedCalculators/CalculatorTypes.h"
#include "GenericConfiguration.h"

class CalculatorConfiguration : public GenericConfiguration {
    private:
        std::string sensorMachineName = "";

    public:
    CalculatorConfiguration() {
    }

    CalculatorConfiguration(std::string sensorMachineName) : CalculatorConfiguration() {
        this->sensorMachineName = sensorMachineName;
    }

    CalculatorConfiguration(std::string sensorMachineName, char * uuid) : CalculatorConfiguration(sensorMachineName) {
        this->uuid = uuid;
    }

    std::string getSensorMachineName() {
        return this->sensorMachineName;
    }

    virtual bool equals(GenericConfiguration * other) {
        CalculatorConfiguration * c = static_cast<CalculatorConfiguration *>(other);
        return this->equals(other);
    }

    virtual bool equals(CalculatorConfiguration * other) {
        if (!GenericConfiguration::equals(other)) {
            return false;
        }

        return this->uuid == other->uuid;
    }

    virtual CalculatorType type() {
        return UNKNOWN_CALCULATOR_TYPE;
    }

    virtual std::vector<SensorType> supportedSensorTypes() {
        std::vector<SensorType> types;
        return types;
    }

    virtual std::string editForm() {
        return "Calculator config for " + std::string(this->uuid) + "<br>";
    }

    virtual JsonDocument toJson() {
        JsonDocument doc = GenericConfiguration::toJson();

        doc["type"] = ToMachineName(this->type());

        return doc;
    }
};

#endif
