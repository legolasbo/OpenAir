#pragma once

#include <string>
#include "../options/Configurable.h"
#include "../enums/CalculatorTypes.h"

class SpeedCalculator : public Configurable {
    protected:
    virtual int _calculate() = 0;
    virtual const char * name() = 0;

    public:
    virtual bool isValid() = 0;
    virtual CalculatorType type() = 0;
    int calculate() {
        if (!this->isValid()) {
            return 0;
        }

        int val = constrain(this->_calculate(), 0, 100);
        Log.verboseln("%s calculated %d", this->name(), val);
        return val;
    }
};

