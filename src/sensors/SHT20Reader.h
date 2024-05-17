#pragma once

#include "Sensor.h"
#include "../Measurements.h"
#include <uFire_SHT20.h>
#include <Wire.h>

struct SHT20Reading {
    float humidity;
    float temperatureDegC;
    float temperatureDegF;
    float dewPointDegC;
    float dewPointDegF;
    float pressure;
};

class SHT20Reader : public Sensor {
    private:
        uFire_SHT20 sht20;
        bool initialized = false;

    public:
        static const SensorType sensorType = SHT20Sensor;
        SensorType getSensorType() override {
            return SHT20Reader::sensorType;
        }
    
        Measurements::MeasurementTypeList getMeasurementTypes() override {
            return Measurements::MeasurementTypeList {
                Measurements::Type::HumidityMeasurement,
                Measurements::Type::TemperatureMeasurement,
                Measurements::Type::DewPointMeasurement,
            };
        }

        Measurements::Measurement provide (Measurements::Type mt) override {
            switch (mt) {
                case Measurements::Type::HumidityMeasurement: return Measurements::Measurement([this]() {
                    return this->getHumidity();
                });
                case Measurements::Type::TemperatureMeasurement: return Measurements::Measurement([this]() {
                    return this->getTemperature();
                });
                case Measurements::Type::DewPointMeasurement: return Measurements::Measurement([this]() {
                    return this->getDewPoint();
                });
                default: return Measurements::Measurement();
            }
        }

        void loop() override {
            if (!this->initialized) {
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
                sht20.begin(SHT20_RESOLUTION_12BITS, SHT20_I2C, i2c);
            }
            if (this->shouldMeasure()) {
                sht20.measure_all();
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

        SHT20Reading takeReading() {
            SHT20Reading r = {
                humidity: this->sht20.RH, 
                temperatureDegC: this->sht20.tempC, 
                temperatureDegF: this->sht20.tempF, 
                dewPointDegC: this->sht20.dew_pointC, 
                dewPointDegF: this->sht20.dew_pointF, 
                pressure: this->sht20.vpd_kPa
            };
            return r;
        }

        float getTemperature() {
            return this->takeReading().temperatureDegC;
        }

        float getHumidity() {
            return this->takeReading().humidity;
        }

        float getDewPoint() {
            return this->takeReading().pressure;
        }

};

