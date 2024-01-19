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
        SelectedMode read();
};

#endif