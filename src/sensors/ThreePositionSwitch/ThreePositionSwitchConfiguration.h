#ifndef THREE_POSITION_SWITCH_CONFIGURATION_H
#define THREE_POSITION_SWITCH_CONFIGURATION_H

#include "../../configuration/SensorConfiguration.h"
#include "../SensorConnectors.h"
#include "../SensorTypes.h"
#include <sstream>

class ThreePositionSwitchConfiguration : public SensorConfiguration {
    public:
        ThreePositionSwitchConfiguration(SensorConnector connector) : SensorConfiguration(connector, ThreePositionSwitch::connectionType, ThreePositionSwitch::sensorType) {}
};

#endif
