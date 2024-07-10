#pragma once

#include "Sensor.h"
#include "../Measurements.h"
#include <SensirionI2CScd4x.h>
#include <Wire.h>
#include <ArduinoLog.h>
#include "../I2CManager.h"
#include "../DependencyInjectionContainer.hpp"

class SCD4x : public Sensor {
    private:
        SensirionI2CScd4x scd4x;
        bool initialized = false;
        bool awaitingData = false;
        uint16_t co2 = 0;
        float temperature = 0.0f;
        float humidity = 0.0f;

    public:
        static const SensorType sensorType = SCD4xSensor;
        SensorType getSensorType() override {
            return SCD4x::sensorType;
        }
    
        Measurements::MeasurementTypeList getMeasurementTypes() override {
            return Measurements::MeasurementTypeList {
                Measurements::Type::CO2Measurement,
                Measurements::Type::HumidityMeasurement,
                Measurements::Type::TemperatureMeasurement,
                Measurements::Type::DewPointMeasurement,
            };
        }

        Measurements::Measurement provide (Measurements::Type mt) override {
            switch (mt) {
                case Measurements::Type::CO2Measurement: return Measurements::Measurement([this]() {
                    return this->getCo2();
                });
                case Measurements::Type::HumidityMeasurement: return Measurements::Measurement([this]() {
                    return this->getHumidity();
                });
                case Measurements::Type::TemperatureMeasurement: return Measurements::Measurement([this]() {
                    return this->getTemperature();
                });
                default: return Measurements::Measurement();
            }
        }

        std::set<std::shared_ptr<HaSensor>> getHaSensors() {
            return {
                std::make_shared<CO2HaSensor>(this->uuid + "-co2", std::string("CO2 (") + this->getName() + ")", [this](){
                    return std::to_string(this->getCo2());
                }),
                std::make_shared<TemperatureHaSensor>(this->uuid + "-temperature", std::string("Temperature (") + this->getName() + ")", [this](){
                    return std::to_string(this->getTemperature());
                }),
                std::make_shared<HumidityHaSensor>(this->uuid + "-humidity", std::string("Humidity (") + this->getName() + ")", [this](){
                    return std::to_string(this->getHumidity());
                }),
            };
        }

        void initialize() {
            ListOption<SensorConnector>* listOptPtr = this->getOption("connector")->as<ListOption<SensorConnector>>();
            if (listOptPtr == nullptr) {
                Log.errorln("Could not cast %s to %s", listOptPtr->typeName().c_str(), typeid(ListOption<SensorConnector>*).name());
                return;
            }

            auto conn = listOptPtr->getValue();
            if (conn == UNKNOWN_CONNECTOR) {
                Log.errorln("Unable to SHT20 sensor: unknown connector.");
                return;
            }

            TwoWire &i2c = DI::GetContainer()->resolve<I2CManager>()->fromConnector(conn); 
            scd4x.begin(i2c);


            uint16_t error;
            char errorMessage[256];
            // stop potentially previously started measurement
            error = scd4x.stopPeriodicMeasurement();
            if (error) {
                Log.errorln("Error trying to execute stopPeriodicMeasurement(): ");
                errorToString(error, errorMessage, 256);
                Log.errorln(errorMessage);
            }

            // Start Measurement
            error = scd4x.startPeriodicMeasurement();
            if (error) {
                Log.errorln("Error trying to execute startPeriodicMeasurement(): ");
                errorToString(error, errorMessage, 256);
                Log.errorln(errorMessage);
            }

            this->initialized = true;
        }

        void loop() override {
            if (!this->initialized) {
                this->initialize();
            }

            if (this->awaitingData || this->shouldMeasure()) {
                this->awaitingData = !this->takeReading();
            }
        }

        std::map<std::string, std::shared_ptr<Option>> availableOptions() override {
            auto options = Sensor::availableOptions();

            std::vector<std::shared_ptr<Option>> defaultConnectorOptions = {
                createOption(X4, ToString(X4)),
                createOption(X6, ToString(X6))
            };
            options.emplace("connector", std::make_shared<ListOption<SensorConnector>>(X4, defaultConnectorOptions, "Connector", true));

            return options;
        }

        std::vector<ConnectionType> getSupportedConnectionTypes() override {
            return {I2C};
        }

        bool takeReading() {
            uint16_t error;
            char errorMessage[256];

            bool isDataReady = false;
            error = scd4x.getDataReadyFlag(isDataReady);
            if (error) {
                Log.warningln("Error trying to execute getDataReadyFlag(): ");
                errorToString(error, errorMessage, 256);
                Log.warningln(errorMessage);
                return false;
            }

            if (!isDataReady) {
                return false;
            }

            error = scd4x.readMeasurement(this->co2, this->temperature, this->humidity);
            if (error) {
                Log.warningln("Error trying to execute readMeasurement(): ");
                errorToString(error, errorMessage, 256);
                Log.warningln(errorMessage);
                return false;
            } 
            
            if (this->co2 == 0) {
                Log.noticeln("Invalid sample detected, skipping.");
            }

            return true;
        }

        float getTemperature() {
            return this->temperature;
        }

        float getHumidity() {
            return this->humidity;
        }

        float getCo2() {
            return this->co2;
        }

};

