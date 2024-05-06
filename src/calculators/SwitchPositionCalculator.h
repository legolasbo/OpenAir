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
};
