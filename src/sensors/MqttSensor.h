#pragma once

#include "Sensor.h"
#include "../MQTT.h"

class MQTTSensor : public Sensor {
    private:
        bool initialized = false;
        float value = 0.0f;

    public:
        static const SensorType sensorType = SensorType::MqttSensor;
        SensorType getSensorType() override {
            return MQTTSensor::sensorType;
        }

        void onMessage(std::string payload) {
            StringOption * key = this->getOption("json_key")->as<StringOption>();
            if (!key->getValue().length()) {
                this->value = std::atof(payload.c_str());
                return;
            }

            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, payload);
            if (err.code() != ArduinoJson::V704PB2::DeserializationError::Ok) {
                Log.errorln("Unable to deserialize payload %s. Reason: %s", payload.c_str(), err.c_str());
                return;
            }
            
            if (!doc.containsKey(key->getValue())) {
                Log.errorln("Unknown key '%s'", key->getValue().c_str());
                return;
            }

            this->value = doc[key->getValue()].as<float>();
        }

        std::string getTopic() {
            StringOption* opt = this->getOption("topic")->as<StringOption>();
            return opt->getValue();
        }

        Measurements::MeasurementTypeList getMeasurementTypes() override {
            std::shared_ptr<Option> opt = this->getOption("measurement_type");
            ListOption<Measurements::Type>* typeOption = opt->as<ListOption<Measurements::Type>>();
            
            switch (typeOption->getValue()) {
                case Measurements::Type::CO2Measurement:
                case Measurements::Type::TemperatureMeasurement:
                case Measurements::Type::HumidityMeasurement:
                    return {typeOption->getValue()};
                default: 
                    return {};
            }
        }

        Measurements::Measurement provide (Measurements::Type mt) override {
            if (!this->getMeasurementTypes().includes(mt)) {
                return Measurements::Measurement();
            }

            switch (mt) {
                case Measurements::Type::HumidityMeasurement: return Measurements::Measurement([this]() {
                    return this->value;
                });
            }
            return Measurements::Measurement();
        }

        std::set<std::shared_ptr<HaSensor>> getHaSensors() {
            auto types = this->getMeasurementTypes();

            std::set<std::shared_ptr<HaSensor>> sensors;
            for (Measurements::Type t : types) {
                std::shared_ptr<HaSensor> s;
                switch (t) {
                    case Measurements::CO2Measurement:
                        s = std::make_shared<CO2HaSensor>(this->uuid + "-co2", std::string("CO2 (" + this->getTopic() + ")"), [this]() {
                            return std::to_string(this->value);
                        });
                        break;
                    case Measurements::HumidityMeasurement:
                        s = std::make_shared<HumidityHaSensor>(this->uuid + "-humidity", std::string("Humidity (" + this->getTopic() + ")"), [this]() {
                            return std::to_string(this->value);
                        });
                        break;
                    case Measurements::TemperatureMeasurement:
                        s = std::make_shared<TemperatureHaSensor>(this->uuid + "-temperature", std::string("Temperature (" + this->getTopic() + ")"), [this]() {
                            return std::to_string(this->value);
                        });
                        break;
                }

                if (s != nullptr) {
                    sensors.emplace(s);
                }
            }
            
            return sensors;
        }

        void initialize() {

            this->initialized = true;
        }

        void loop() override {
            if (!this->initialized) {
                this->initialize();
            }

        }

        std::map<std::string, std::shared_ptr<Option>> availableOptions() override {
            auto options = Sensor::availableOptions();

            options.emplace("topic", std::make_shared<StringOption>("", "Topic", true));

            std::vector<std::shared_ptr<Option>> availableMeasurementTypes = {
                createOption(Measurements::Type::CO2Measurement, Measurements::ToString(Measurements::Type::CO2Measurement)),
                createOption(Measurements::Type::DewPointMeasurement, Measurements::ToString(Measurements::Type::DewPointMeasurement)),
                createOption(Measurements::Type::HumidityMeasurement, Measurements::ToString(Measurements::Type::HumidityMeasurement)),
                createOption(Measurements::Type::PressureMeasurement, Measurements::ToString(Measurements::Type::PressureMeasurement)),
                createOption(Measurements::Type::SwitchPositionMeasurement, Measurements::ToString(Measurements::Type::SwitchPositionMeasurement)),
                createOption(Measurements::Type::TemperatureMeasurement, Measurements::ToString(Measurements::Type::TemperatureMeasurement)),
            };
            options.emplace("measurement_type", std::make_shared<ListOption<Measurements::Type>>(Measurements::Type::HumidityMeasurement, availableMeasurementTypes, "Measurement type", true));

            options.emplace("json_key", std::make_shared<StringOption>("", "JSON Key", true));
            return options;
        }

        std::vector<ConnectionType> getSupportedConnectionTypes() override {
            return {MQTT_CONNECTION};
        }

};
