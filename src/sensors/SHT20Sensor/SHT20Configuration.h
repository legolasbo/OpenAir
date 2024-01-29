#ifndef SHT20_CONFIGURATION_H
#define SHT20_CONFIGURATION_H

#include "../../configuration/SensorConfiguration.h"
#include "../SensorConnectors.h"
#include "../SensorTypes.h"
#include <sstream>

class SHT20Configuration : public SensorConfiguration {

    public:
    SHT20Configuration(SensorConnector connector) : SensorConfiguration(connector, SHT20Reader::connectionType, SHT20Reader::sensorType) {}
};


#endif
