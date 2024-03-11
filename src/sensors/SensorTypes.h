#ifndef SENSOR_TYPES_H
#define SENSOR_TYPES_H
#include <vector>

enum SensorType {
    UNKNOWN_SENSOR_TYPE,
    ThreePositionSwitchSensor,
    SHT20Sensor,
};

const char* ToString(SensorType v);

const char* ToMachineName(SensorType v);

SensorType SensorTypeFromMachineName(const char * name);

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

#endif