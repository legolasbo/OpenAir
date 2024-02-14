#ifndef SENSOR_TYPES_H
#define SENSOR_TYPES_H

enum SensorType {
    UNKNOWN_SENSOR_TYPE,
    ThreePositionSwitchSensor,
    SHT20Sensor,
};

const char* ToString(SensorType v);

const char* ToMachineName(SensorType v);

SensorType SensorTypeFromMachineName(const char * name);

#endif