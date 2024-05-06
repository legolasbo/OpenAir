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

    public:
        static const SensorType sensorType = SHT20Sensor;
        SensorType getSensorType() {
            return SHT20Reader::sensorType;
        }
    
        Measurements::MeasurementTypeList getMeasurementTypes() {
            return Measurements::MeasurementTypeList {
                Measurements::Type::HumidityMeasurement,
                Measurements::Type::TemperatureMeasurement,
                Measurements::Type::DewPointMeasurement,
            };
        }

        Measurements::Measurement provide (Measurements::Type mt) {
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

        void loop() {
            if (this->shouldMeasure()) {
                sht20.measure_all();
            }
        }

        std::unordered_map<std::string, std::shared_ptr<Option>> availableOptions() {
            auto options = Sensor::availableOptions();

            std::vector<Option> defaultConnectorOptions = {Option(X4, ToString(X4)), Option(X6, ToString(X6))};
            options.emplace("connector", std::make_shared<ListOption>(X4, defaultConnectorOptions, "Connector", true));

            return options;
        }

        std::vector<ConnectionType> getSupportedConnectionTypes() {
            return {I2C};
        }

        SHT20Reading takeReading() {
            
            SHT20Reading result = {
                humidity: sht20.RH, 
                temperatureDegC: sht20.tempC, 
                temperatureDegF: sht20.tempF, 
                dewPointDegC: sht20.dew_pointC, 
                dewPointDegF: sht20.dew_pointF, 
                pressure: sht20.vpd_kPa
            };

            return result;
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

