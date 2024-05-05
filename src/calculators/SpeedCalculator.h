#pragma once

#include <string>
#include "../options/Configurable.h"
#include "../enums/CalculatorTypes.h"

class SpeedCalculator : public Configurable {
    protected:
    virtual int _calculate() = 0;
    virtual const char * name() = 0;

    public:
    virtual bool isValid() = 0;
    virtual CalculatorType type() = 0;
    virtual Measurements::MeasurementTypeList supportedMeasurementTypes() = 0;

    virtual std::unordered_map<std::string, std::shared_ptr<Option>> availableOptions() {
        auto supportedSensors = DI::GetContainer()->resolve<SensorRepository>()->getSensorsSupportingMeasurements(this->supportedMeasurementTypes());
        std::vector<Option> sensorOptions;
        for (auto s : supportedSensors) {
            sensorOptions.push_back(Option(s->getUuid(), s->getName()));
        }

        return {
            {"name", std::make_shared<Option>(this->name(), "Name", true)},
            {"sensor", std::make_shared<ListOption>("", sensorOptions, "Sensor", true)},
        };
    }

    virtual JsonDocument toInterfaceOptions() {
        JsonDocument doc = Configurable::toInterfaceOptions();

        doc["type"]["type"] = "hidden";
        doc["type"]["value"] = ToMachineName(this->type());

        return doc;
    }

    virtual JsonDocument toJson() {
        JsonDocument doc = Configurable::toJson();

        doc["type"] = ToMachineName(this->type());

        return doc;
    }
    
    int calculate() {
        if (!this->isValid()) {
            return 0;
        }

        int val = constrain(this->_calculate(), 0, 100);
        Log.verboseln("%s calculated %d", this->name(), val);
        return val;
    }
};

