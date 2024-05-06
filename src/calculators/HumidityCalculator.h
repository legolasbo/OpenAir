#pragma once

#include <stdexcept>
#include "../Measurements.h"
#include "SpeedCalculator.h"
#include "../sensors/sensor.h"
#include "../repositories/SensorRepository.hpp"
  
class HumidityCalculator : public SpeedCalculator {  

    protected:
        virtual int _calculate() {
            return this->getSensor()->provide(Measurements::HumidityMeasurement).measure();
        }
        virtual const char * name() {
            return "Humidity";
        }

    public:
        virtual bool isValid() {
            return this->getSensor() != nullptr;
        }

        virtual CalculatorType type() {
            return HUMIDITY_CALCULATOR;
        }

        virtual Measurements::MeasurementTypeList supportedMeasurementTypes() {
            return {Measurements::HumidityMeasurement};
        }

};
