#pragma once

#include "I2CSensor.h"
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

class SHT20Reader : public I2CSensor, 
                    public Measurements::Temperature,
                    public Measurements::Humidity,
                    public Measurements::DewPoint
                    {
    private:
        uFire_SHT20 sht20;
        u_long lastReadMillis;

        bool shouldMeasure() {
            u_long now;

            if (now < lastReadMillis) {
                return true;
            }

            return lastReadMillis - now >= 1000;
        }

    public:
        static const SensorType sensorType = SHT20Sensor;

        SHT20Reader() : I2CSensor(Sensor::generateUuid(), Wire){}
        SHT20Reader(std::string uuid, TwoWire &bus) : I2CSensor(uuid, bus) {
            sht20.begin(SHT20_RESOLUTION_12BITS, SHT20_I2C, bus);
        }
        
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

        std::unordered_map<std::string, Option> availableOptions() {
            return {};
        }

        SHT20Reading takeReading() {
            if (this->shouldMeasure()) {
                sht20.measure_all();
            }
            
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

