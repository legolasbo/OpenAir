#pragma once

#include "../calculators/SpeedCalculator.h"
#include "../configuration/CalculatorConfigurations.h"

class CalculatorRepository : public Repository<SpeedCalculator> {
    public: 
    CalculatorRepository (): Repository<SpeedCalculator>() {}

    std::shared_ptr<SpeedCalculator> fromUuid(std::string uuid) {
        std::shared_ptr<SpeedCalculator> calculator = this->getInstance(uuid);
        if (calculator != nullptr) {
            return calculator;
        }

        CalculatorConfiguration * config = DI::GetContainer()->resolve<CalculatorConfigurations>()->get(uuid);
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

