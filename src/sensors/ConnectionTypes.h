#ifndef CONNECTION_TYPES_H
#define CONNECTION_TYPES_H

enum ConnectionType {
    UNKNOWN_CONNECTION_TYPE,
    I2C,
    UART,
};

const char* ToMachineName(ConnectionType);

const char* ToString(ConnectionType);

ConnectionType ConnectionTypeFromMachineName(const char * name);

#endif