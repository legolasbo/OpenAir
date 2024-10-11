#pragma once

#include <stdexcept>
#include "../Measurements.h"
#include "SpeedCalculator.h"
#include "../sensors/sensor.h"
#include "../repositories/SensorRepository.hpp"
#include <options/Option.h>
  
class DifferenceCalculator : public SpeedCalculator {  
    private:
        std::shared_ptr<Sensor> secondarySensor = nullptr;

        void setSecondarySensor(const std::string &uuid) {
            this->secondarySensor = DI::GetContainer()->resolve<SensorRepository>()->getInstance(uuid);
        }

    protected:
        int _calculate() override {
            int primary = this->getSensor()->provide(Measurements::HumidityMeasurement).measure();
            int secondary = this->getSecondarySensor()->provide(Measurements::HumidityMeasurement).measure();
            int raw = abs(primary - secondary);

            RangeOption* from = this->getOption("from")->as<RangeOption>();
            RangeOption* to = this->getOption("to")->as<RangeOption>();

            return map(raw, from->lower(), from->upper(), to->lower(), to->upper());
        }

        const char * name() override {
            return "Mapping";
        }

        std::shared_ptr<Sensor> getSecondarySensor() {
            return this->secondarySensor;
        }

    public:
        bool isValid() override {
            auto primary = this->getSensor();
            auto secondary = this->getSecondarySensor();
            if (primary == nullptr) {
                Log.infoln("Invalid, primary nullptr");
                return false;
            } 
            if (secondary == nullptr) {
                Log.infoln("Invalid, secondary nullptr");
                return false;
            }

            auto supported = this->supportedMeasurementTypes();
            if (!primary->getMeasurementTypes().intersects(supported)) {
                Log.infoln("Invalid, primary unsupported measurement");
                return false;
            }
            if (!secondary->getMeasurementTypes().intersects(supported)) {
                Log.infoln("Invalid, secondary unsupported measurement");
                return false;
            }

            return true;
        }


        bool _setOption(const std::string &name, std::shared_ptr<Option> value) override {
            bool outcome = SpeedCalculator::_setOption(name, value);

            if (name == "secondary_sensor") {
                this->setSecondarySensor(value->toStr());
            }

            return outcome;
        }

        std::map<std::string, std::shared_ptr<Option>> availableOptions() override {
            auto options = SpeedCalculator::availableOptions();

             auto supportedSensors = DI::GetContainer()->resolve<SensorRepository>()->getSensorsSupportingMeasurements(this->supportedMeasurementTypes());
            std::vector<std::shared_ptr<Option>> sensorOptions;
            for (auto s : supportedSensors) {
                sensorOptions.push_back(createOption(s->getUuid(), s->getName()));
            }
            
            options.emplace("from", std::make_shared<RangeOption>(0, 0, 100, 100, "From"));
            options.emplace("to", std::make_shared<RangeOption>(0, 0, 100, 100, "To"));
            options.emplace("secondary_sensor", std::make_shared<ListOption<std::string>>("", sensorOptions, "Secondary sensor", true));

            return options;
        }

        CalculatorType type() override {
            return DIFFERENCE_CALCULATOR;
        }

        Measurements::MeasurementTypeList supportedMeasurementTypes() override {
            return {
                Measurements::HumidityMeasurement
            };
        }

};
