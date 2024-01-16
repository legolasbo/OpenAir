#include <Arduino.h>
#include "SpeedCalculator.h"

void SpeedCalculator::print()  {
    Serial.print(name().c_str());
    Serial.print(": ");
    Serial.println(calculate());
}

std::string SpeedCalculator::name() {
    throw std::runtime_error("NOT IMPLEMENTED");
}

int SpeedCalculator::calculate() {
    throw std::runtime_error("NOT IMPLEMENTED");
}
