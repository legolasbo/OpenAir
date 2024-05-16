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

        std::string getPositionOptionName(int position) {
            char buffer [100];
            snprintf(buffer, 100, "position_%d", position);
            return std::string(buffer);
        }

        std::string getPositionOptionLabel(int position) {
            char buffer[100];
            snprintf(buffer, 100, "Fan speed percentage at position %d", position);
            return std::string(buffer);
        }
    
    protected:
        int _calculate() override {
            auto measurement = this->getSensor()->provide(Measurements::SwitchPositionMeasurement);
            int position = measurement.measure();
            std::string optionName = this->getPositionOptionName(position);
            if (!this->isAvailableOption(optionName)) {
                Log.traceln("Position %d is not a configurable option", position);
                return 0;
            }

            return this->getOption(optionName)->as<IntegerOption>()->getValue();
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

        std::map<std::string, std::shared_ptr<Option>> availableOptions() override {
            auto options = SpeedCalculator::availableOptions();

            auto sensor = this->getSensor();
            if (sensor && sensor->getMeasurementTypes().includes(Measurements::Type::SwitchPositionCountMeasurement)) {
                this->initializePositionMap();

                int numpos = sensor->provide(Measurements::SwitchPositionCountMeasurement).measure();

                for (int i = 1; i <= numpos; i++) {
                    options.emplace(this->getPositionOptionName(i), new BoundedOption(positionMap[i], 0, 100, this->getPositionOptionLabel(i), true));
                }
            }

            return options;
        }

        JsonDocument toInterfaceOptions() override {
            JsonDocument doc = SpeedCalculator::toInterfaceOptions();

            return doc;
        }
};
