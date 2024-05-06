#pragma once

#include <stdexcept>
#include "../Measurements.h"
#include "SpeedCalculator.h"
#include "../sensors/sensor.h"
#include "../repositories/SensorRepository.hpp"
  
class HumidityCalculator : public SpeedCalculator {  

    protected:
        int _calculate() override {
            return this->getSensor()->provide(Measurements::HumidityMeasurement).measure();
        }
        const char * name() override {
            return "Humidity";
        }

    public:
        bool isValid() override {
            return this->getSensor() != nullptr;
        }

        CalculatorType type() override {
            return HUMIDITY_CALCULATOR;
        }

        Measurements::MeasurementTypeList supportedMeasurementTypes() override {
            return {Measurements::HumidityMeasurement};
        }

};
