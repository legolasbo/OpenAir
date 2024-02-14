#ifndef SENSOR_CONNECTORS_H
#define SENSOR_CONNECTORS_H

enum SensorConnector {
    UNKNOWN_CONNECTOR,
    X4,
    X6,
};

inline const char* ToString(SensorConnector c) {
    switch (c) {
        case X4: return "x4";
        case X6: return "x6";
        default: return "Unknown connector";
    }
}

inline const char* ToMachineName(SensorConnector c) {
    switch (c) {
        case X4: return "x4";
        case X6: return "x6";
        default: return "Unknown_connector";
    }
}

SensorConnector SensorConnectorFromMachineName(const char * name) {
    if (strcmp(name, ToMachineName(X4)) == 0) {
        return X4;
    }
    if (strcmp(name, ToMachineName(X6)) == 0) {
        return X6;
    }

    return UNKNOWN_CONNECTOR;
}

#endif
