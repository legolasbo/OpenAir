#pragma once

#include <string>
#include "../options/Configurable.h"

class SpeedCalculator : public Configurable {
    protected:
    virtual int _calculate() = 0;
    virtual const char * name() = 0;

    public:
    virtual bool isValid() = 0;
    int calculate() {
        if (!this->isValid()) {
            return 0;
        }

        int val = constrain(this->_calculate(), 0, 100);
        Log.verboseln("%s calculated %d", this->name(), val);
        return val;
    }
};

