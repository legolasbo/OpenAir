#include "SpeedCalculator.h"
#include "../sensors/Sensor.h"
#include "../Measurements.h"

class SwitchPositionCalculator : public SpeedCalculator {
    private:
        Measurements::SwitchPosition * sensor;
        std::vector<int> positionMap;

        void initializePositionMap() {
            int defaultIncrement = 100 / this->sensor->getNumberOfPositions();
            this->positionMap.push_back(0);
            for (int i  = 1; i <= this->sensor->getNumberOfPositions(); i++) {
                this->positionMap.push_back(i * defaultIncrement);
            }
        }

        int mapPositionToValue(uint8_t position) {
            return this->positionMap[position];
        }
    
    protected:
        int _calculate() {
            uint8_t position = this->sensor->getSelectedPosition();
            if (position > this->sensor->getNumberOfPositions()) {
                position = this->sensor->getNumberOfPositions();
            }

            return this->mapPositionToValue(position);
        }
        const char * name() {
            return "Switch position";
        }

    public:
        SwitchPositionCalculator(Measurements::SwitchPosition * sensor) {
            this->sensor = sensor;
            this->initializePositionMap();
        }

};
