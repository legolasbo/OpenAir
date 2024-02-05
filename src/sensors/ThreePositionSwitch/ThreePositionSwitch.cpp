#include "ThreePositionSwitch.h"

SensorType ThreePositionSwitch::getSensorType() {
    return ThreePositionSwitch::sensorType;
}

SelectedMode ThreePositionSwitch::read() {
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