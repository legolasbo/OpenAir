#ifndef ModeSelect_H
#define ModeSelect_H

#include <Wire.h>

enum SelectedMode {
  MODE_LOW = 1,
  MODE_MEDIUM = 2,
  MODE_HIGH = 3
};

class ModeSelectSwitch {
    private:
        TwoWire *i2cBus;

    public:
        ModeSelectSwitch(TwoWire *bus) {
            i2cBus = bus;
        }
        SelectedMode read() {
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
};

#endif