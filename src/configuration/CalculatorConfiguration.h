#pragma once

#include <string>
#include <vector>
#include "../Measurements.h"
#include "../enums/CalculatorTypes.h"
#include "../calculators/SpeedCalculator.h"
#include "GenericConfiguration.h"

class CalculatorConfiguration : public GenericConfiguration {

    public:
    CalculatorConfiguration(std::shared_ptr<DI> container) : GenericConfiguration(container) {}

    virtual CalculatorType type() {
        return UNKNOWN_CALCULATOR_TYPE;
    }

    virtual bool dependsOn(std::string uuid) {
        return false;
    }

    virtual Measurements::MeasurementTypeList supportedMeasurementTypes() = 0;

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

    virtual std::shared_ptr<SpeedCalculator> createInstance() = 0;
};

