#ifndef SENSOR_BASED_CALCULATOR_CONFIGURATION
#define SENSOR_BASED_CALCULATOR_CONFIGURATION

#include "CalculatorConfiguration.h"
#include "SensorConfigurations.h"
#include "../speedCalculators/CalculatorTypes.h"
#include <vector>
#include <map>
#include "../sensors/SensorTypes.h"


class SensorBasedCalculatorConfiguration : public CalculatorConfiguration {
    private:
        std::string sensorUuid;

        bool isValidSensor(std::string uuid) {
            SensorConfiguration * sensor = this->sensorConfigs->get(uuid);
            if (sensor == nullptr) {
                return false;
            }
            return this->supportedSensorTypes().includes(sensor->getSensorType());
        }

    public:

    SensorBasedCalculatorConfiguration(SensorConfigurations * sensorConfigs) : CalculatorConfiguration(sensorConfigs){}

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
        if (!this->isValidSensor(value)) {
            return false;
        }
        this->sensorUuid = value;
        return true;
    }

    virtual bool isValid() {
        return this->sensorConfigs->get(this->sensorUuid) != nullptr;
    }
    
    virtual bool setOption(std::string name, int value) {
        return CalculatorConfiguration::setOption(name, value);
    }

    virtual bool setOption(std::string name, std::string value) {
        if (name == "sensor") {
            return this->setSensor(value);
        }
        return CalculatorConfiguration::setOption(name, value);
    }

    virtual JsonDocument getConfigurationOptions() {
        JsonDocument doc = CalculatorConfiguration::getConfigurationOptions();

        auto supportedTypes = this->supportedSensorTypes();
        
        if (supportedTypes.size() > 0) {
            doc["sensor"]["type"] = "select";
            doc["sensor"]["label"] = "Sensor";

            std::map<std::string, SensorType> uuidMap = this->sensorConfigs->getUuidsForTypes(supportedTypes);

            for (auto entry : uuidMap) {
                std::string name = this->sensorConfigs->get(entry.first)->getName();
                if (name == "") {
                    name.append("Unnamed ");
                    name.append(ToString(entry.second));
                }

                doc["sensor"]["options"][entry.first] = name;
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
