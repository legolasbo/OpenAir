#ifndef CALCULATOR_FACTORY_H
#define CALCULATOR_FACTORY_H

#include "../sensors/SensorFactory.h"
#include "../speedCalculators/SpeedCalculator.h"
#include "../configuration/CalculatorConfigurations.h"

class CalculatorFactory : public Factory<SpeedCalculator> {
    private:
    SensorFactory * sensorFactory;
    CalculatorConfigurations * configs;

    public: 
    CalculatorFactory (SensorFactory * sensorFactory, CalculatorConfigurations * configs) {
        this->sensorFactory = sensorFactory;
        this->configs = configs;
    }

    SpeedCalculator * fromUuid(std::string uuid) {
        SpeedCalculator * calculator = this->getInstance(uuid);
        if (calculator != nullptr) {
            return calculator;
        }

        CalculatorConfiguration * config = this->configs->get(uuid);
        if (config == nullptr) {
            return nullptr;
        }

        SpeedCalculator * instance = config->createInstance(this->sensorFactory);
        this->registerInstance(uuid, instance);
        return instance;
    }    

};

#endif
