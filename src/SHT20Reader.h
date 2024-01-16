#include <uFire_SHT20.h>

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
        int sda, scl;
        uFire_SHT20 sht20;

    public:
        SHT20Reader(int sdaPin, int sclPin) {
            sda = sdaPin;
            scl = sclPin;
        }

        SHT20Reading takeReading();
};
