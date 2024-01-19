#include <Wire.h>
#include <Arduino.h>
#include "ModeSelect.h"

SelectedMode ModeSelectSwitch::read() {
    i2cBus->begin();
    SelectedMode mode = MODE_LOW;
    
    try {
        i2cBus->requestFrom(4, 1);
        if (i2cBus->available()) {
        mode = (SelectedMode) i2cBus->read();
        }

    }
    catch(const std::exception& e) {
        Serial.println(e.what());
    }
    
    i2cBus->end();
    return mode;
}
