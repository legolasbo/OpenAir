#pragma once

#include "Measurements.h"
#include "../enums/SensorTypes.h"
#include "../enums/ConnectionTypes.h"
#include "../options/Configurable.h"

class Sensor : public Configurable, public Measurements::MeasurementProvider {
    private:
        u_long lastReadMillis = 0;

    protected:
        bool shouldMeasure() {
            u_long now = millis();

            if (now < lastReadMillis || now - lastReadMillis >= 1000) {
                lastReadMillis = now;
                return true;
            }

            return false;
        }

    public:
        Sensor() : Configurable() {}
        Sensor(std::string uuid) : Configurable(uuid) {}

        virtual ~Sensor() = default;
        virtual SensorType getSensorType() = 0;
        virtual std::vector<ConnectionType> getSupportedConnectionTypes() = 0;
        virtual void loop() = 0;

        std::string getName() {
            return this->getOption("name")->toStr();
        }

        std::unordered_map<std::string, std::shared_ptr<Option>> availableOptions() override {
            std::unordered_map<std::string, std::shared_ptr<Option>> options;
            options.emplace("name", std::make_shared<Option>(ToString(this->getSensorType()), "Name", true));

            std::vector<ConnectionType> supportedTypes = this->getSupportedConnectionTypes();
            if (supportedTypes.size() == 0) {
                return options;
            }

            std::vector<Option> supportedTypeOptions;
            for (ConnectionType t : supportedTypes) {
                supportedTypeOptions.push_back(Option(t));
            }

            options.emplace("connection", std::make_shared<ListOption>(supportedTypes.at(0), supportedTypeOptions, "Connection", true));

            return options;
        }

        JsonDocument toInterfaceOptions() override {
            JsonDocument doc = Configurable::toInterfaceOptions();

            doc["type"]["type"] = "hidden";
            doc["type"]["value"] = ToMachineName(this->getSensorType());

            return doc;
        }

        JsonDocument toJson() override {
            JsonDocument doc = Configurable::toJson();
            
            doc["type"] = ToMachineName(this->getSensorType());

            return doc;
        }
};

