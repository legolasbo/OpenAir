#pragma once

#include "string.h"

enum ConnectionType {
    UNKNOWN_CONNECTION_TYPE,
    I2C,
    UART,
};

const char* ToMachineName(ConnectionType v) {
    switch (v)
    {
        case I2C:     return "i2c";
        case UART:    return "uart";
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
    
    return UNKNOWN_CONNECTION_TYPE;
}
