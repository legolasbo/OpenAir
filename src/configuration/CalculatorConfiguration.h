#ifndef CALCULATOR_CONFIGURATION_H
#define CALCULATOR_CONFIGURATION_H

#include <string>
#include <vector>
#include "../sensors/SensorTypes.h"

class CalculatorConfiguration {
    private:
        std::string sensorMachineName;

    public:
    CalculatorConfiguration(std::string sensorMachineName) {
        this->sensorMachineName = sensorMachineName;
    }

    std::string getSensorMachineName() {
        return this->sensorMachineName;
    }

    bool equals(CalculatorConfiguration * other) {
        if (this->sensorMachineName != other->getSensorMachineName()) {
            return false;
        }

        return true;
    }

    virtual std::vector<SensorType> supportedSensorTypes() {
        std::vector<SensorType> types;
        return types;
    }
};

#endif
