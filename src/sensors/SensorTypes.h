#ifndef SENSOR_TYPES_H
#define SENSOR_TYPES_H

enum SensorType {
    UNKNOWN_SENSOR_TYPE,
    ThreePositionSwitchSensor,
    SHT20Sensor,
};

inline const char* ToString(SensorType v) {
    switch (v)
    {
        case ThreePositionSwitchSensor:     return "Three position switch";
        case SHT20Sensor:                   return "SHT20 temperature/humidity sensor";
        default:                            return "[Unknown sensor]";
    }
}

inline const char* ToMachineName(SensorType v) {
    switch (v)
    {
        case ThreePositionSwitchSensor:     return "3possw";
        case SHT20Sensor:                   return "sht20";
        default:                            return "unknown";
    }
}

#endif