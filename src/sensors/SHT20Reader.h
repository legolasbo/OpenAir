#ifndef SHT20Reader_H
#define SHT20Reader_H

#include "I2CSensor.h"
#include "SensorTypes.h"
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

class SHT20Reader : public  I2CSensor, 
                            Measurements::Temperature,
                            Measurements::Humidity,
                            Measurements::DewPoint
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

        SHT20Reader(TwoWire *bus) : I2CSensor(bus) {
            sht20.begin(SHT20_RESOLUTION_12BITS, SHT20_I2C, *bus);
        }
        
        virtual SensorType getSensorType() {
            return SHT20Reader::sensorType;
        }
    
        virtual std::vector<Measurements::Type> getMeasurementTypes() {
            return std::vector<Measurements::Type> {
                Measurements::Type::HumidityMeasurement,
                Measurements::Type::TemperatureMeasurement,
                Measurements::Type::DewPointMeasurement,
            };
        }

        virtual SHT20Reading takeReading() {
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

        virtual float getTemperature() {
            return this->takeReading().temperatureDegC;
        }

        virtual float getHumidity() {
            return this->takeReading().humidity;
        }

        virtual float getDewPoint() {
            return this->takeReading().pressure;
        }

};

#endif
