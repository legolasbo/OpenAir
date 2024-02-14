#include "SensorTypes.h"
#include <string.h> 

const char* ToString(SensorType v) {
    switch (v)
    {
        case ThreePositionSwitchSensor:     return "Three position switch";
        case SHT20Sensor:                   return "SHT20 temperature/humidity sensor";
        default:                            return "[Unknown sensor]";
    }
}

const char* ToMachineName(SensorType v) {
    switch (v)
    {
        case ThreePositionSwitchSensor:     return "3possw";
        case SHT20Sensor:                   return "sht20";
        default:                            return "unknown";
    }
}

SensorType SensorTypeFromMachineName(const char * name) {
    if (strcmp(name, ToMachineName(ThreePositionSwitchSensor)) == 0) {
        return ThreePositionSwitchSensor;
    }
    if (strcmp(name, ToMachineName(SHT20Sensor)) == 0) {
        return SHT20Sensor;
    }

    return UNKNOWN_SENSOR_TYPE;
}
