#ifndef ModeSelect_H
#define ModeSelect_H

#include <Arduino.h>
#include "I2CSensor.h"
#include <Wire.h>
#include "SensorTypes.h"

enum SelectedMode {
  MODE_LOW = 1,
  MODE_MEDIUM = 2,
  MODE_HIGH = 3
};

#define THREE_POSITION_SWITCH_ADDRESS 4

class ThreePositionSwitch : public I2CSensor {
    public:
        ThreePositionSwitch(TwoWire *i2cBus) : I2CSensor(i2cBus) {}

        SelectedMode read() {
            this->i2cBus->begin();
            SelectedMode mode = MODE_LOW;
            
            try {
                this->i2cBus->requestFrom(THREE_POSITION_SWITCH_ADDRESS, 1);
                if (this->i2cBus->available()) {
                mode = (SelectedMode) this->i2cBus->read();
                }

            }
            catch(const std::exception& e) {
                Serial.println(e.what());
            }
            
            this->i2cBus->end();
            return mode;
        }

        SensorType getType() {
            return ThreePositionSwitchSensor;
        }
};

#endif