#pragma once

#include <stdexcept>
#include "../Measurements.h"
#include "SpeedCalculator.h"
#include "../sensors/sensor.h"
#include "../repositories/SensorRepository.hpp"
  
class Co2Calculator : public SpeedCalculator {  

    protected:
        int _calculate() override {
            int co2 = this->getSensor()->provide(Measurements::CO2Measurement).measure();
            int lowerbound = this->getOption("lowerbound")->as<BoundedOption>()->getValue();
            int upperbound = this->getOption("upperbound")->as<BoundedOption>()->getValue();
            return map(co2, lowerbound, upperbound, 10, 100);
        }
        const char * name() override {
            return "Co2";
        }

    public:
        bool isValid() override {
            if (this->getSensor() == nullptr) {
                return false;
            }

            return this->getSensor()->getMeasurementTypes().intersects(this->supportedMeasurementTypes());
        }

        std::map<std::string, std::shared_ptr<Option>> availableOptions() override {
            auto options = SpeedCalculator::availableOptions();
            
            options.emplace("lowerbound", std::make_shared<BoundedOption>(400, 0, 800, "Lowerbound", true));
            options.emplace("upperbound", std::make_shared<BoundedOption>(900, 800, 2000, "Upperbound", true));

            return options;
        }

        CalculatorType type() override {
            return CO2_CALCULATOR;
        }

        Measurements::MeasurementTypeList supportedMeasurementTypes() override {
            return {
                Measurements::CO2Measurement
            };
        }

};
