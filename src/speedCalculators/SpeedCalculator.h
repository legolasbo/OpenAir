#ifndef SPEEDCALCULATOR_H
#define SPEEDCALCULATOR_H

#include <string>

class SpeedCalculator {
    public:

    void print()  {
        Serial.println("printing");
        Serial.println(this->calculate());
    }

    virtual std::string name() {
        return "NOT IMPLEMENTED";
    }

    virtual int calculate() {
        return 0;
    }

};

#endif
