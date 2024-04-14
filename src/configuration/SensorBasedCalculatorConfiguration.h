#ifndef SENSOR_BASED_CALCULATOR_CONFIGURATION
#define SENSOR_BASED_CALCULATOR_CONFIGURATION

#include "CalculatorConfiguration.h"
#include "SensorConfigurations.h"
#include "../speedCalculators/CalculatorTypes.h"
#include "../sensors/SensorTypes.h"
#include "../factories/SensorFactory.h"


class SensorBasedCalculatorConfiguration : public CalculatorConfiguration {
    private:
        std::string sensorUuid;

    protected:
    std::string getSensorUuid() {
        return this->sensorUuid;
    }

    public:
    SensorBasedCalculatorConfiguration(DI * container) : CalculatorConfiguration(container) {}

    virtual bool dependsOn(std::string uuid) {
        return sensorUuid == uuid;
    }

    virtual bool hasOption(std::string name) {
        if (name == "sensor") {
            return true;
        }

        return CalculatorConfiguration::hasOption(name);
    }

    bool setSensor(std::string value) {
        this->sensorUuid = value;
        this->markDirty();
        return true;
    }

    virtual bool isValid() {
        return this->container->resolve<SensorFactory>()->fromUuid(this->sensorUuid) != nullptr;
    }
    
    virtual bool oldSetOption(std::string name, int value) {
        return CalculatorConfiguration::oldSetOption(name, value);
    }

    virtual bool oldSetOption(std::string name, std::string value) {
        Serial.printf("Setting option '%s' to value '%s'\n", name.c_str(), value.c_str());
        if (name == "sensor") {
            return this->setSensor(value);
        }
        return CalculatorConfiguration::oldSetOption(name, value);
    }

    virtual JsonDocument getConfigurationOptions() {
        JsonDocument doc = CalculatorConfiguration::getConfigurationOptions();

        auto supportedTypes = this->supportedMeasurementTypes();
        auto sensors = this->container->resolve<SensorFactory>()->getSensorsSupportingMeasurements(supportedTypes);
        
        if (supportedTypes.size() > 0) {
            doc["sensor"]["type"] = "select";
            doc["sensor"]["label"] = "Sensor";

            for (auto sensor : sensors) {
                SensorConfiguration* sensorConfig = this->container->resolve<SensorConfigurations>()->get(sensor->getUuid());
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

    virtual JsonDocument toJson() {
        JsonDocument doc = CalculatorConfiguration::toJson();

        doc["sensor"] = this->sensorUuid;

        return doc;
    }

    virtual JsonDocument toDetails() {
        JsonDocument doc = CalculatorConfiguration::toDetails();        

        doc["sensor"]["label"] = "Sensor";
        doc["sensor"]["value"] = this->sensorUuid;
        doc["sensor"]["type"] = "sensor";

        return doc;
    }

};

#endif
