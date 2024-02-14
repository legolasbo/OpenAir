#ifndef SHT20Reader_H
#define SHT20Reader_H

#include "../I2CSensor.h"
#include "../SensorTypes.h"
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

class SHT20Reader : public I2CSensor {
    private:
        uFire_SHT20 sht20;

    public:
        static const SensorType sensorType = SHT20Sensor;

        SHT20Reader(TwoWire *bus) : I2CSensor(bus) {
            sht20.begin(SHT20_RESOLUTION_12BITS, SHT20_I2C, *bus);
        }
        
        SensorType getSensorType();
        SHT20Reading takeReading();
};

#endif
