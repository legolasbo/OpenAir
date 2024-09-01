#pragma once

#include <stdexcept>
#include "../Measurements.h"
#include "SpeedCalculator.h"
#include "../sensors/sensor.h"
#include "../repositories/SensorRepository.hpp"
  
class MappingCalculator : public SpeedCalculator {  

    protected:
        int _calculate() override {
            int raw = this->getSensor()->provide(Measurements::HumidityMeasurement).measure();
            RangeOption* from = this->getOption("from")->as<RangeOption>();
            RangeOption* to = this->getOption("to")->as<RangeOption>();

            return map(raw, from->lower(), from->upper(), to->lower(), to->upper());
        }

        const char * name() override {
            return "Mapping";
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
            
            options.emplace("from", std::make_shared<RangeOption>(0, 0, 100, 100, "From"));
            options.emplace("to", std::make_shared<RangeOption>(0, 0, 100, 100, "To"));

            return options;
        }

        CalculatorType type() override {
            return MAPPING_CALCULATOR;
        }

        Measurements::MeasurementTypeList supportedMeasurementTypes() override {
            return {
                Measurements::HumidityMeasurement
            };
        }

};
