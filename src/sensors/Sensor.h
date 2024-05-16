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
        Sensor(const std::string &uuid) : Configurable(uuid) {}

        virtual ~Sensor() = default;
        virtual SensorType getSensorType() = 0;
        virtual std::vector<ConnectionType> getSupportedConnectionTypes() = 0;
        virtual void loop() = 0;

        std::string getName() {
            return this->getOption("name")->toStr();
        }

        std::map<std::string, std::shared_ptr<Option>> availableOptions() override {
            std::map<std::string, std::shared_ptr<Option>> options;
            options.emplace("name", createOption(ToString(this->getSensorType()), "Name", true));

            std::vector<ConnectionType> supportedTypes = this->getSupportedConnectionTypes();
            if (supportedTypes.size() == 0) {
                return options;
            }

            std::vector<std::shared_ptr<Option>> supportedTypeOptions;
            for (ConnectionType t : supportedTypes) {
                supportedTypeOptions.push_back(createOption(t));
            }

            options.emplace("connection", std::make_shared<ListOption<ConnectionType>>(supportedTypes.at(0), supportedTypeOptions, "Connection", true));

            return options;
        }

        JsonDocument toInterfaceOptions() override {
            JsonDocument doc = Configurable::toInterfaceOptions();

            JsonObject opt = doc["options"].add<JsonObject>();
            opt["name"] = "type";
            opt["info"]["type"] = "hidden";
            opt["info"]["value"] = ToMachineName(this->getSensorType());

            return doc;
        }

        JsonDocument toJson() override {
            JsonDocument doc = Configurable::toJson();
            
            doc["type"] = ToMachineName(this->getSensorType());

            return doc;
        }
};

