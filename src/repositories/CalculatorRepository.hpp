#pragma once

#include "../calculators/Co2Calculator.h"
#include "../calculators/DifferenceCalculator.h"
#include "../calculators/HumidityCalculator.h"
#include "../calculators/MappingCalculator.h"
#include "../calculators/SwitchPositionCalculator.h"
#include "../calculators/UnknownCalculator.h"
#include "../enums/CalculatorTypes.h"

class CalculatorRepository : public Repository<SpeedCalculator> {
    public: 
    CalculatorRepository (): Repository<SpeedCalculator>() {}

    std::shared_ptr<SpeedCalculator> create(CalculatorType type) {
        switch (type) {
            case CO2_CALCULATOR:                return std::make_shared<Co2Calculator>();
            case DIFFERENCE_CALCULATOR:         return std::make_shared<DifferenceCalculator>();
            case HUMIDITY_CALCULATOR:           return std::make_shared<HumidityCalculator>();
            case MAPPING_CALCULATOR:            return std::make_shared<MappingCalculator>();
            case SWITCH_POSITION_CALCULATOR:    return std::make_shared<SwitchPositionCalculator>();
            default: return std::make_shared<UnknownCalculator>();
        }
    }

    std::shared_ptr<SpeedCalculator> create(const std::string &type) override {
        return this->create(CalculatorTypeFromMachineName(type));
    }

    JsonDocument availableCalculatorTypes() {
        JsonDocument doc;

        Measurements::MeasurementTypeList measurementTypes = DI::GetContainer()->resolve<SensorRepository>()->availableMeasurementTypes();
        auto repo = DI::GetContainer()->resolve<CalculatorRepository>();

        auto next = CalculatorType(0);
        do {
            auto calc = repo->create(next);
            if (calc->supportedMeasurementTypes().intersects(measurementTypes)) {
                doc[ToMachineName(next)] = ToString(next);
            }

            next = CalculatorType(int(next) + 1);
        }
        while (next != UNKNOWN_CALCULATOR_TYPE);

        return doc;
    }
};

