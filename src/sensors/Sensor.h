#pragma once

#include "Measurements.h"
#include "../enums/SensorTypes.h"
#include "../enums/ConnectionTypes.h"
#include "../options/Configurable.h"

class Sensor : public Configurable, public Measurements::Measurement {
    public:
        Sensor() : Configurable() {}
        Sensor(std::string uuid) : Configurable(uuid) {}

        virtual ~Sensor() = default;
        virtual SensorType getSensorType() = 0;
        virtual Measurements::MeasurementTypeList getMeasurementTypes() = 0;
        virtual std::vector<ConnectionType> getSupportedConnectionTypes() = 0;

        std::string getName() {
            return this->getOption("name")->toStr();
        }

        bool supportsMeasurementType(Measurements::Type type) {
            for (Measurements::Type t : this->getMeasurementTypes()) {
                if (t == type) {
                    return true;
                }
            }
            return false;
        }

        template <typename T>
        std::shared_ptr<T> toMeasurement() {
            std::shared_ptr<T> measurement = std::shared_ptr<T>(dynamic_cast<T*>(this));
            if (measurement == nullptr) {
                Log.errorln("Could not cast sensor to %s Actual type %s", typeid(T*).name(), typeid(*this).name());
                return nullptr;
            }
            return measurement;
        }

        std::unordered_map<std::string, std::shared_ptr<Option>> availableOptions() {
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

        virtual JsonDocument toInterfaceOptions() {
            JsonDocument doc = Configurable::toInterfaceOptions();

            doc["type"]["type"] = "hidden";
            doc["type"]["value"] = ToMachineName(this->getSensorType());

            return doc;
        }

        virtual JsonDocument toJson() {
            JsonDocument doc = Configurable::toJson();
            
            doc["type"] = ToMachineName(this->getSensorType());

            return doc;
        }
};

