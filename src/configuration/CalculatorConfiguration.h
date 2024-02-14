#ifndef CALCULATOR_CONFIGURATION_H
#define CALCULATOR_CONFIGURATION_H

#include <string>
#include <vector>
#include "../sensors/SensorTypes.h"
#include "GenericConfiguration.h"
#include "UUID.h"

class CalculatorConfiguration : public GenericConfiguration {
    private:
        std::string uuid;
        std::string sensorMachineName = "";

    public:
    CalculatorConfiguration() {
        UUID uuid;
        uuid.seed(esp_random(), esp_random());
        uuid.setRandomMode();
        uuid.generate();
        this->uuid = std::string(uuid.toCharArray());
    }

    CalculatorConfiguration(std::string sensorMachineName) : CalculatorConfiguration() {
        this->sensorMachineName = sensorMachineName;
    }

    CalculatorConfiguration(std::string sensorMachineName, char * uuid) {
        this->sensorMachineName = sensorMachineName;
        this->uuid = uuid;
    }

    std::string getUuid() {
        return this->uuid;
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

    virtual std::vector<SensorType> supportedSensorTypes() {
        std::vector<SensorType> types;
        return types;
    }

    virtual std::string editForm() {
        return "Calculator config for " + this->uuid + "<br>";
    }
};

#endif
