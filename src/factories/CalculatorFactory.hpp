#ifndef CALCULATOR_FACTORY_H
#define CALCULATOR_FACTORY_H

#include "../speedCalculators/SpeedCalculator.h"
#include "../configuration/CalculatorConfigurations.h"

class CalculatorFactory : public Factory<SpeedCalculator> {
    public: 
    CalculatorFactory (DI * container): Factory<SpeedCalculator>(container) {}

    SpeedCalculator * fromUuid(std::string uuid) {
        SpeedCalculator * calculator = this->getInstance(uuid);
        if (calculator != nullptr) {
            return calculator;
        }

        CalculatorConfiguration * config = this->container->resolve<CalculatorConfigurations>()->get(uuid);
        if (config == nullptr || !config->isValid()) {
            return nullptr;
        }

        SpeedCalculator * instance = config->createInstance();
        if (instance != nullptr) {
            this->registerInstance(uuid, instance);
        }
        return instance;
    }    

};

#endif
