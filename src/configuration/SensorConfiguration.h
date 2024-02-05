#ifndef SENSOR_CONFIGURATION_H
#define SENSOR_CONFIGURATION_H

#include "../sensors/SensorConnectors.h"
#include "../sensors/SensorTypes.h"
#include "../sensors/ConnectionTypes.h"
#include <sstream>

class SensorConfiguration {
    private:
        SensorConnector    connector;
        SensorType          sensorType;
        ConnectionType      connectionType;


    public:
    SensorConfiguration(SensorConnector connector, ConnectionType connectionType, SensorType sensorType) {
        this->connector = connector;
        this->connectionType = connectionType;
        this->sensorType = sensorType;
    }

    SensorConnector getSensorConnector() {
        return this->connector;
    }

    SensorType getSensorType() {
        return this->sensorType;
    }

    ConnectionType getConnectionType() {
        return this->connectionType;
    }

    bool equals(SensorConfiguration *other) {
        if (this->sensorType != other->sensorType) {
            return false;
        }

        if (this->connector != other->connector) {
            return false;
        }

        if (this->connectionType != other->connectionType) {
            return false;
        }

        return true;
    }

    bool equals(SensorConfiguration &other) {
        return this->equals(&other);
    }

    std::string getMachineName() {
        std::ostringstream out;

        out << ToMachineName(this->connector);
        out << "_";
        out << ToMachineName(this->connectionType);
        out << "_";
        out << ToMachineName(this->sensorType);

        return out.str();
    }
};


#endif
