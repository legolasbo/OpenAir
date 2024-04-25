#ifndef CALCULATOR_FACTORY_H
#define CALCULATOR_FACTORY_H

#include "../calculators/SpeedCalculator.h"
#include "../configuration/CalculatorConfigurations.h"

class CalculatorFactory : public Factory<SpeedCalculator> {
    public: 
    CalculatorFactory (std::shared_ptr<DI> container): Factory<SpeedCalculator>(container) {}

    std::shared_ptr<SpeedCalculator> fromUuid(std::string uuid) {
        std::shared_ptr<SpeedCalculator> calculator = this->getInstance(uuid);
        if (calculator != nullptr) {
            return calculator;
        }

        CalculatorConfiguration * config = this->container->resolve<CalculatorConfigurations>()->get(uuid);
        if (config == nullptr || !config->isValid()) {
            return nullptr;
        }

        std::shared_ptr<SpeedCalculator> instance = config->createInstance();
        if (instance != nullptr) {
            this->registerInstance(uuid, instance);
        }
        return instance;
    }    

};

#endif
