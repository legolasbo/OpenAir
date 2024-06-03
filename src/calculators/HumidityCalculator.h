#pragma once

#include <stdexcept>
#include "../Measurements.h"
#include "SpeedCalculator.h"
#include "../sensors/sensor.h"
#include "../repositories/SensorRepository.hpp"
  
class HumidityCalculator : public SpeedCalculator {  

    protected:
        int _calculate() override {
            int humidity = this->getSensor()->provide(Measurements::HumidityMeasurement).measure();
            int offset = this->getOption("offset")->as<BoundedOption>()->getValue();
            return humidity + offset;
        }
        const char * name() override {
            return "Humidity";
        }

    public:
        bool isValid() override {
            return this->getSensor() != nullptr;
        }

        std::map<std::string, std::shared_ptr<Option>> availableOptions() override {
            auto options = SpeedCalculator::availableOptions();
            
            options.emplace("offset", std::make_shared<BoundedOption>(-30, -100, 100, "Humidity\% + offset = speed\%", true));

            return options;
        }

        CalculatorType type() override {
            return HUMIDITY_CALCULATOR;
        }

        Measurements::MeasurementTypeList supportedMeasurementTypes() override {
            return {Measurements::HumidityMeasurement};
        }

};
