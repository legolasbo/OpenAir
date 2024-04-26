#pragma once

#include "../calculators/HumidityCalculator.h"
#include "../calculators/SwitchPositionCalculator.h"
#include "../calculators/UnknownCalculator.h"
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

    std::shared_ptr<SpeedCalculator> create(CalculatorType type) {
        switch (type) {
            case HUMIDITY_CALCULATOR: return std::make_shared<HumidityCalculator>();
            case SWITCH_POSITION_CALCULATOR: return std::make_shared<SwitchPositionCalculator>();
            default: return std::make_shared<UnknownCalculator>();
        }
    }

    std::shared_ptr<SpeedCalculator> create(std::string type) {
        return this->create(CalculatorTypeFromMachineName(type));
    }
};

