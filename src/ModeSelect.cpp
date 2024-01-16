#include <Wire.h>
#include <Arduino.h>
#include "ModeSelect.h"

SelectedMode ModeSelectSwitch::read() {
    Wire.begin(sda, scl);
    SelectedMode mode = MODE_LOW;
    
    try {
        Wire.requestFrom(4, 1);
        if (Wire.available()) {
        mode = (SelectedMode) Wire.read();
        }

    }
    catch(const std::exception& e) {
        Serial.println(e.what());
    }
    
    Wire.end();
    return mode;
}
