#pragma once

#include "SpeedCalculator.h"
#include "../sensors/Sensor.h"
#include "../Measurements.h"

class SwitchPositionCalculator : public SpeedCalculator {
    private:
        char const * timedName = "timed";
        char const * minPerSecName = "minutes_per_second";
        char const * manualAfterSecName = "manual_after_seconds";
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

        int previousPosition = 1;
        int timedPosition = 1;
        int secondsToTime = 0;
        unsigned long inPositionSince = millis();
        bool manualMode = true;
        int getPosition() {
            auto measurement = this->getSensor()->provide(Measurements::SwitchPositionMeasurement);
            bool timed = this->getOption(timedName)->as<BooleanOption>()->getValue();
            int position = measurement.measure();

            if (!timed) {
                return position;
            }


            unsigned long now = millis();
            unsigned long millisElapsed = now < inPositionSince ? ULONG_MAX - inPositionSince + now : now - inPositionSince;
            unsigned long secondsElapsed = millisElapsed / 1000;
            int manualAfterSeconds = this->getOption(manualAfterSecName)->as<BoundedOption>()->getValue();

            if (position == previousPosition) {
                if (position == 1) {
                    if (secondsToTime > secondsElapsed) {
                        return timedPosition;
                    }
                    secondsToTime = 0;
                    return position;
                }

                if (!manualMode) {
                    manualMode = secondsElapsed > manualAfterSeconds;
                }
                return position;
            }

            timedPosition = previousPosition;
            previousPosition = position;
            inPositionSince = now;
            if (manualMode) {
                manualMode = false;
                secondsToTime = 0;
                return position;
            }

            int minutesPerSecond = this->getOption(minPerSecName)->as<BoundedOption>()->getValue();
            Log.infoln("Starting timer of %d minutes for position %d", secondsElapsed * minutesPerSecond, timedPosition);
            secondsToTime = secondsElapsed * minutesPerSecond * 60;
            return timedPosition;

        }
    
    protected:
        int _calculate() override {
            int position = this->getPosition();
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
                    options.emplace(this->getPositionOptionName(i), std::make_shared<BoundedOption>(positionMap[i], 0, 100, this->getPositionOptionLabel(i), true));
                }
            }

            options.emplace(timedName, std::make_shared<BooleanOption>(true, "Enable timed position", true));
            options.emplace(minPerSecName, std::make_shared<BoundedOption>(10, 1, 60, "Number of minutes per second", true));
            options.emplace(manualAfterSecName, std::make_shared<BoundedOption>(60, 1, 3600, "Speed is considered manual after seconds", true));

            return options;
        }

        JsonDocument toInterfaceOptions() override {
            JsonDocument doc = SpeedCalculator::toInterfaceOptions();

            return doc;
        }
};
