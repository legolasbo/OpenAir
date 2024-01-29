#ifndef SENSOR_CONNECTORS_H
#define SENSOR_CONNECTORS_H

enum SensorConnector {
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

#endif
