#pragma once

#include <string>

class SpeedCalculator {
    protected:
    virtual int _calculate() = 0;
    virtual const char * name() = 0;

    public:
    int calculate() {
        int val = constrain(this->_calculate(), 0, 100);
        Log.verboseln("%s calculated %d", this->name(), val);
        return val;
    }
};

