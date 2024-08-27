#pragma once

#include "string.h"

enum ConnectionType {
    UNKNOWN_CONNECTION_TYPE,
    I2C,
    UART,
    MQTT_CONNECTION,
};

const char* ToMachineName(ConnectionType v) {
    switch (v)
    {
        case I2C:     return "i2c";
        case UART:    return "uart";
        case MQTT_CONNECTION:    return "mqtt";
        default:      return "[unknown connection type]";
    }
}

const char* ToString(ConnectionType v) {
    return ToMachineName(v);
}

ConnectionType ConnectionTypeFromMachineName(const char * name) {
    if (strcmp(name, ToMachineName(I2C)) == 0) {
        return I2C;
    }
    if (strcmp(name, ToMachineName(UART)) == 0) {
        return UART;
    }
    if (strcmp(name, ToMachineName(MQTT_CONNECTION)) == 0) {
        return MQTT_CONNECTION;
    }
    
    return UNKNOWN_CONNECTION_TYPE;
}
