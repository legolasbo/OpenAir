#pragma once

#include "SpeedCalculator.h"
#include "../sensors/Sensor.h"
#include "../Measurements.h"

class SwitchPositionCalculator : public SpeedCalculator {
    private:
        std::vector<int> positionMap;

        std::shared_ptr<Measurements::SwitchPosition> getSensor() {
            auto sensor = DI::GetContainer()->resolve<SensorRepository>()->getInstance(this->getOption("sensor"));
            return sensor->toMeasurement<Measurements::SwitchPosition>();
        }

        void initializePositionMap() {
            if (!this->isValid()) {
                return;
            }

            auto sensor = this->getSensor();
            if (sensor->getNumberOfPositions() != this->positionMap.size() + 1) {
                this->positionMap.clear();
            }

            int defaultIncrement = 100 / sensor->getNumberOfPositions();
            this->positionMap.push_back(0);
            for (int i  = 1; i <= sensor->getNumberOfPositions(); i++) {
                this->positionMap.push_back(i * defaultIncrement);
            }
        }

        const int mapPositionToValue(uint8_t position) {
            this->initializePositionMap();
            return this->positionMap[position];
        }
    
    protected:
        virtual bool isValid() {
            return this->getSensor() != nullptr;
        }

        virtual int _calculate() {
            uint8_t position = this->getSensor()->getSelectedPosition();
            if (position > this->getSensor()->getNumberOfPositions()) {
                position = this->getSensor()->getNumberOfPositions();
            }

            return this->mapPositionToValue(position);
        }

        virtual const char * name() {
            return "Switch position";
        }

        virtual std::unordered_map<std::string, Option> availableOptions() {
            return {
                {"name", Option(this->name())},
                {"sensor", Option()},
            };
        }

    public:
        SwitchPositionCalculator() {}

};
