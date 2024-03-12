#ifndef THREE_POSITION_SWITCH_CALCULATOR_CONFIGURATION
#define THREE_POSITION_SWITCH_CALCULATOR_CONFIGURATION

#include "SensorBasedCalculatorConfiguration.h"
#include "../speedCalculators/CalculatorTypes.h"
#include <vector>
#include "../sensors/SensorTypes.h"
#include "../enums.h"

class ThreePositionSwitchCalculatorConfiguration : public SensorBasedCalculatorConfiguration {
    private:
    int min = FanPercentages::OFF;
    int med = FanPercentages::HALF;
    int max = FanPercentages::MAX;

    std::string formatPercentage(int value) {
        int val = value;
        if (val < 0) {
            val = 0;
        }
        if (val > 100) {
            val = 100;
        } 
        char buff[4];
        sprintf(buff, "%d%", value);

        return buff;
    }

    public:
    ThreePositionSwitchCalculatorConfiguration(SensorConfigurations * sensorConfigs) : SensorBasedCalculatorConfiguration(sensorConfigs) {}

    virtual SensorTypeList supportedSensorTypes() {
        return SensorTypeList {
            ThreePositionSwitchSensor,
        };
    }

    virtual CalculatorType type() {
        return ThreePositionSwitchCalculator;
    }

    virtual bool hasOption(std::string name) {
        if (
            name == "min" ||
            name == "med" ||
            name == "max"
        ) {
            return true;
        }

        return SensorBasedCalculatorConfiguration::hasOption(name);
    }

    virtual bool setOption(std::string name, std::string value) {
        if (name == "min" || name == "med" || name == "max") {
            return this->setOption(name, strtol(value.c_str(), nullptr, 10));
        }

        return SensorBasedCalculatorConfiguration::setOption(name, value);
    }

    virtual bool setOption(std::string name, int value) {
        if (name == "min") {
            return this->setMin(value);
        }

        if (name == "med") {
            return this->setMed(value);
        }

        if (name == "max") {
            return this->setMax(value);
        }

        return SensorBasedCalculatorConfiguration::setOption(name, value);
    }

    bool setMin(int value) {
        this->min = constrain(value, FanPercentages::OFF, FanPercentages::MINIMUM_UPPER_LIMIT);
        if (this->max < this->min) {
            this->setMax(this->min);
        }
        if (this->med < this->min) {
            this->setMed(this->min);
        }
        return true;
    }

    bool setMed(int value) {
        this->med = constrain(value, FanPercentages::MEDIUM_LOWER_LIMIT, FanPercentages::MEDIUM_UPPER_LIMIT);
        if (this->min > this->med) {
            this->setMin(this->med);
        }
        if (this->max < this->med) {
            this->setMax(this->med);
        }
        return true;
    }

    bool setMax(int value) {
        this->max = constrain(value, FanPercentages::MAXIMUM_LOWER_LIMIT, FanPercentages::MAX);
        if (this->med > this->max) {
            this->setMed(this->max);
        }
        if (this->min > this->max) {
            this->setMin(this->max);
        }
        return true;
    }

    virtual JsonDocument getConfigurationOptions() {
        JsonDocument doc = SensorBasedCalculatorConfiguration::getConfigurationOptions();

        doc["min"]["type"] = "number";
        doc["min"]["label"] = "Minimum fan speed (%)";
        doc["min"]["constrain"]["min"] = FanPercentages::OFF;
        doc["min"]["constrain"]["max"] = FanPercentages::MINIMUM_UPPER_LIMIT;

        doc["med"]["type"] = "number";
        doc["med"]["label"] = "Medium fan speed (%)";
        doc["med"]["constrain"]["min"] = FanPercentages::MEDIUM_LOWER_LIMIT;
        doc["med"]["constrain"]["max"] = FanPercentages::MEDIUM_UPPER_LIMIT;

        doc["max"]["type"] = "number";
        doc["max"]["label"] = "Maximum fan speed (%)";
        doc["max"]["constrain"]["min"] = FanPercentages::MAXIMUM_LOWER_LIMIT;
        doc["max"]["constrain"]["max"] = FanPercentages::MAX;

        return doc;
    }

    virtual JsonDocument toJson() {
        JsonDocument doc =  SensorBasedCalculatorConfiguration::toJson();

        doc["min"] = this->min;
        doc["med"] = this->med;
        doc["max"] = this->max;

        return doc;
    }

    virtual JsonDocument toDetails() {
        JsonDocument doc = SensorBasedCalculatorConfiguration::toDetails();

        doc["min"]["label"] = "Minimum fan speed";
        doc["min"]["value"] = this->formatPercentage(this->min);
        doc["min"]["type"] = "key/value";
        doc["med"]["label"] = "Medium fan speed";
        doc["med"]["value"] = this->formatPercentage(this->med);
        doc["med"]["type"] = "key/value";
        doc["max"]["label"] = "Maximum fan speed";
        doc["max"]["value"] = this->formatPercentage(this->max);
        doc["max"]["type"] = "key/value";

        return doc;
    }
};

#endif
