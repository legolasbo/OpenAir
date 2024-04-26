#pragma once

#include <stdexcept>
#include "../Measurements.h"
#include "SpeedCalculator.h"
#include "../sensors/sensor.h"
#include "../repositories/SensorRepository.hpp"
  
class HumidityCalculator : public SpeedCalculator {  
    private:
        std::shared_ptr<Measurements::Humidity> getSensor() {
            auto sensor = DI::GetContainer()->resolve<SensorRepository>()->getInstance(this->getOption("sensor"));
            return sensor->toMeasurement<Measurements::Humidity>();
        }

    protected:
        virtual int _calculate() {
            return (int) this->getSensor()->getHumidity() - 40;
        }
        virtual const char * name() {
            return "Humidity";
        }

    public:
        virtual bool isValid() {
            return this->getSensor() != nullptr;
        }

        virtual std::unordered_map<std::string, Option> availableOptions() {
            return {
                {"name", Option(this->name())},
                {"sensor", Option()},
            };
        }

};
