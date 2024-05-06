#pragma once

#include <vector>
#include <string.h> 

enum SensorType {
    UNKNOWN_SENSOR_TYPE,
    ThreePositionSwitchSensor,
    SHT20Sensor,
};

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

SensorType SensorTypeFromMachineName(const std::string &name) {
    if (name == ToMachineName(ThreePositionSwitchSensor)) {
        return ThreePositionSwitchSensor;
    }
    if (name == ToMachineName(SHT20Sensor)) {
        return SHT20Sensor;
    }

    return UNKNOWN_SENSOR_TYPE;
}

class SensorTypeList : public std::vector<SensorType> {
    public:
    SensorTypeList() : std::vector<SensorType>(){}
    SensorTypeList(std::initializer_list<SensorType> s) : std::vector<SensorType>(s) {}
    
    bool includes(SensorType needle) {
        for (size_t i = 0; i < this->size(); i++) {
            if (this->at(i) == needle) {
                return true;
            }
        }
        return false;
    }

    bool intersects(SensorTypeList other) {
        for(SensorType t : other) {
            if (this->includes(t)) {
                return true;
            }
        }
        return false;
    }
};
