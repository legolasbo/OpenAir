#ifndef SHT20Reader_H
#define SHT20Reader_H

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

class SHT20Reader {
    private:
        TwoWire *i2cBus;
        uFire_SHT20 sht20;

    public:
        SHT20Reader(TwoWire *bus) {
            i2cBus = bus;
            sht20.begin(SHT20_RESOLUTION_12BITS, SHT20_I2C, *i2cBus);
        }

        SHT20Reading takeReading();
};

#endif
