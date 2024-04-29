#pragma once

#include "CalculatorConfiguration.h"
#include "../enums/CalculatorTypes.h"
#include "../repositories/SensorRepository.hpp"

class SensorBasedCalculatorConfiguration : public CalculatorConfiguration {

    public:
    SensorBasedCalculatorConfiguration() : CalculatorConfiguration() {}


    virtual std::unordered_map<std::string, std::shared_ptr<Option>> availableOptions() {
        auto options = CalculatorConfiguration::availableOptions();
        options.emplace("sensor", std::make_shared<Option>(""));
        return options;
    };

    virtual bool dependsOn(std::string uuid) {
        return this->getOption("sensor")->toStr() == uuid;
    }

    bool setSensor(std::string value) {
        this->setOption("sensor", value);
        this->markDirty();
        return true;
    }

    virtual bool isValid() {
        return DI::GetContainer()->resolve<SensorRepository>()->getInstance(this->getOption("sensor")->toStr()) != nullptr;
    }

    virtual JsonDocument getConfigurationOptions() {
        JsonDocument doc = CalculatorConfiguration::getConfigurationOptions();

        auto supportedTypes = this->supportedMeasurementTypes();
        auto sensors = DI::GetContainer()->resolve<SensorRepository>()->getSensorsSupportingMeasurements(supportedTypes);
        
        if (supportedTypes.size() > 0) {
            doc["sensor"]["type"] = "select";
            doc["sensor"]["label"] = "Sensor";

            for (auto sensor : sensors) {
                doc["sensor"]["options"][sensor->getUuid()] = sensor->getUuid();
            }
        }

        return doc;
    }

    virtual JsonDocument toDetails() {
        JsonDocument doc = CalculatorConfiguration::toDetails();        

        doc["sensor"]["label"] = "Sensor";
        doc["sensor"]["value"] = this->getOption("sensor")->toStr();
        doc["sensor"]["type"] = "sensor";

        return doc;
    }

};
