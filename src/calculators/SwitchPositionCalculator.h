#pragma once

#include "SpeedCalculator.h"
#include "../sensors/Sensor.h"
#include "../Measurements.h"

class SwitchPositionCalculator : public SpeedCalculator {
    private:
        std::vector<int> positionMap;

        void initializePositionMap() {
            if (!this->isValid()) {
                return;
            }

            int numPos = this->getSensor()->provide(Measurements::SwitchPositionCountMeasurement).measure();
            if (numPos != this->positionMap.size() + 1) {
                this->positionMap.clear();
            }

            int defaultIncrement = 100 / numPos;
            this->positionMap.push_back(0);
            for (int i  = 1; i <= numPos; i++) {
                this->positionMap.push_back(i * defaultIncrement);
            }
        }

        const int mapPositionToValue(int position) {
            this->initializePositionMap();

            int mapSize = this->positionMap.size() - 1;

            position = max(0, min(mapSize, position));
            return this->positionMap[position];
        }
    
    protected:
        int _calculate() override {
            auto measurement = this->getSensor()->provide(Measurements::SwitchPositionMeasurement);
            int position = measurement.measure();
            return this->mapPositionToValue(position);
        }

        const char * name() override {
            return "Switch position";
        }

    public:
        CalculatorType type() override {
            return SWITCH_POSITION_CALCULATOR;
        }

        bool isValid() override {
            return this->getSensor() != nullptr;
        }

        Measurements::MeasurementTypeList supportedMeasurementTypes() override {
            return {Measurements::SwitchPositionMeasurement};
        }

        std::unordered_map<std::string, std::shared_ptr<Option>> availableOptions() override {
            auto options = SpeedCalculator::availableOptions();

            auto sensor = this->getSensor();
            if (sensor && sensor->getMeasurementTypes().includes(Measurements::Type::SwitchPositionCountMeasurement)) {
                this->initializePositionMap();

                int numpos = sensor->provide(Measurements::SwitchPositionCountMeasurement).measure();
                // auto group = std::make_shared<OptionGroup>("Position config");
                for (int i = 1; i <= numpos; i++) {
                    std::string name = "position_" + i;
                    options.emplace(name, createOption(this->positionMap[i], "Position " + i));
                }

                // options.emplace("position_config", group);
            }

            return options;
        }

        JsonDocument toInterfaceOptions() override {
            JsonDocument doc = SpeedCalculator::toInterfaceOptions();

            return doc;
        }
};
