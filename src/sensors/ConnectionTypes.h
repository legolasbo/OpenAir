#ifndef CONNECTION_TYPES_H
#define CONNECTION_TYPES_H


enum ConnectionType {
    UNKNOWN_CONNECTION_TYPE,
    I2C,
    UART,
};

inline const char* ToMachineName(ConnectionType v) {
    switch (v)
    {
        case I2C:     return "i2c";
        case UART:    return "uart";
        default:      return "[unknown connection type]";
    }
}

inline const char* ToString(ConnectionType v) {
    return ToMachineName(v);
}


#endif