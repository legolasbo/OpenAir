#pragma once

#include <string>
#include "../options/Configurable.h"
#include "../enums/CalculatorTypes.h"

class SpeedCalculator : public Configurable {
    private:
    std::shared_ptr<Sensor> sensor = nullptr;

    void setSensor(const std::string &uuid) {
        this->sensor = DI::GetContainer()->resolve<SensorRepository>()->getInstance(uuid);
    }

    protected:
    virtual int _calculate() = 0;
    virtual const char * name() = 0;

    std::shared_ptr<Sensor> getSensor() {
        return this->sensor;
    }

    bool _setOption(const std::string &name, std::shared_ptr<Option> value) override {
        bool outcome = Configurable::_setOption(name, value);

        if (name == "sensor") {
            this->setSensor(value->toStr());
        }

        return outcome;
    }

    public:
    virtual bool isValid() = 0;
    virtual CalculatorType type() = 0;
    virtual Measurements::MeasurementTypeList supportedMeasurementTypes() = 0;

    std::unordered_map<std::string, std::shared_ptr<Option>> availableOptions() override {
        auto supportedSensors = DI::GetContainer()->resolve<SensorRepository>()->getSensorsSupportingMeasurements(this->supportedMeasurementTypes());
        std::vector<std::shared_ptr<Option>> sensorOptions;
        for (auto s : supportedSensors) {
            sensorOptions.push_back(createOption(s->getUuid(), s->getName()));
        }

        return {
            {"name", createOption(this->name(), "Name")},
            {"sensor", std::make_shared<ListOption<std::string>>("", sensorOptions, "Sensor", true)},
        };
    }

    JsonDocument toInterfaceOptions() override {
        JsonDocument doc = Configurable::toInterfaceOptions();

        doc["type"]["type"] = "hidden";
        doc["type"]["value"] = ToMachineName(this->type());

        return doc;
    }

    JsonDocument toJson() override {
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

