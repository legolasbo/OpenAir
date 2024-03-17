#ifndef SPEEDCALCULATOR_H
#define SPEEDCALCULATOR_H

#include <string>

class SpeedCalculator {
    public:

    void print()  {
        Serial.println("printing");
        Serial.println(this->calculate());
    }

    virtual int calculate() = 0;
};

#endif
