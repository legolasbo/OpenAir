#pragma once

#include "CalculatorConfiguration.h"
#include "SensorConfigurations.h"
#include "../enums/CalculatorTypes.h"
#include "../factories/SensorFactory.h"


class SensorBasedCalculatorConfiguration : public CalculatorConfiguration {

    public:
    SensorBasedCalculatorConfiguration() : CalculatorConfiguration() {}


    virtual std::unordered_map<std::string, Option> availableOptions() {
        auto options = CalculatorConfiguration::availableOptions();
        options.emplace("sensor", Option(""));
        return options;
    };

    virtual bool dependsOn(std::string uuid) {
        return this->getOption("sensor").toStr() == uuid;
    }

    bool setSensor(std::string value) {
        this->setOption("sensor", value);
        this->markDirty();
        return true;
    }

    virtual bool isValid() {
        return DI::GetContainer()->resolve<SensorFactory>()->fromUuid(this->getOption("sensor").toStr()) != nullptr;
    }

    virtual JsonDocument getConfigurationOptions() {
        JsonDocument doc = CalculatorConfiguration::getConfigurationOptions();

        auto supportedTypes = this->supportedMeasurementTypes();
        auto sensors = DI::GetContainer()->resolve<SensorFactory>()->getSensorsSupportingMeasurements(supportedTypes);
        
        if (supportedTypes.size() > 0) {
            doc["sensor"]["type"] = "select";
            doc["sensor"]["label"] = "Sensor";

            for (auto sensor : sensors) {
                SensorConfiguration* sensorConfig = DI::GetContainer()->resolve<SensorConfigurations>()->get(sensor->getUuid());
                std::string name = "";
                if (sensorConfig != nullptr) {
                    name = sensorConfig->getName();
                }
                if (name == "") {
                    name.append("Unnamed ");
                    name.append(ToString(sensor->getSensorType()));
                }
                doc["sensor"]["options"][sensor->getUuid()] = name;
            }
        }

        return doc;
    }

    virtual JsonDocument toDetails() {
        JsonDocument doc = CalculatorConfiguration::toDetails();        

        doc["sensor"]["label"] = "Sensor";
        doc["sensor"]["value"] = this->getOption("sensor").toStr();
        doc["sensor"]["type"] = "sensor";

        return doc;
    }

};
