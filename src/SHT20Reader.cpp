#include "SHT20Reader.h"


SHT20Reading SHT20Reader::takeReading() {
    sht20.measure_all();
    
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