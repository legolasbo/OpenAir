#ifndef CALCULATOR_CONFIGURATION_H
#define CALCULATOR_CONFIGURATION_H

#include <string>
#include <vector>
#include "../sensors/SensorTypes.h"
#include "../speedCalculators/CalculatorTypes.h"
#include "GenericConfiguration.h"

class CalculatorConfiguration : public GenericConfiguration {
    private:
        std::string name = "";
    
    protected:
        std::vector<std::string> sensorDependencies;

    public:
    CalculatorConfiguration() : GenericConfiguration() {}
    CalculatorConfiguration(const char * uuid): GenericConfiguration(uuid) {}
    CalculatorConfiguration(const char * name, const char * uuid) : CalculatorConfiguration(uuid) {
        this->name = name;
    }

    std::vector<std::string> getSensorDependencies() {
        return this->sensorDependencies;
    }

    virtual bool equals(GenericConfiguration * other) {
        CalculatorConfiguration * c = static_cast<CalculatorConfiguration *>(other);
        return this->equals(other);
    }

    virtual bool equals(CalculatorConfiguration * other) {
        if (!GenericConfiguration::equals(other)) {
            return false;
        }

        return this->name == other->name;
    }

    std::string getName() {
        return this->name;
    }

    virtual CalculatorType type() {
        return UNKNOWN_CALCULATOR_TYPE;
    }

    virtual std::vector<SensorType> supportedSensorTypes() {
        std::vector<SensorType> types;
        return types;
    }

    virtual JsonDocument toJson() {
        JsonDocument doc = GenericConfiguration::toJson();

        doc["type"] = ToMachineName(this->type());
        doc["name"] = this->name;

        return doc;
    }
};

#endif
