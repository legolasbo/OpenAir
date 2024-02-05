#ifndef THREE_POSITION_SWITCH_H
#define THREE_POSITION_SWITCH_H

#include <Arduino.h>
#include "../I2CSensor.h"
#include <Wire.h>

enum SelectedMode {
  MODE_LOW = 1,
  MODE_MEDIUM = 2,
  MODE_HIGH = 3
};

#define THREE_POSITION_SWITCH_ADDRESS 4

class ThreePositionSwitch : public I2CSensor {
    public:
        ThreePositionSwitch(TwoWire *i2cBus) : I2CSensor(i2cBus) {}

        static const SensorType sensorType = ThreePositionSwitchSensor;
        SensorType getSensorType();

        SelectedMode read();
};

#endif