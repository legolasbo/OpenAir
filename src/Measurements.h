#pragma once

#include <set>

namespace Measurements {

    enum Type {
        TemperatureMeasurement,
        HumidityMeasurement,
        DewPointMeasurement,
        PressureMeasurement,
        SwitchPositionMeasurement,
    };

    class MeasurementTypeList : public std::set<Type> {
        public:
        MeasurementTypeList() : std::set<Type>(){}
        MeasurementTypeList(std::initializer_list<Type> s) : std::set<Type>(s) {}
        
        bool includes(Type needle) {
            auto iterator = this->find(needle);
            if (iterator != this->end()) {
                return true;
            }
            return false;
        }

        bool includesAll(MeasurementTypeList other) {
            for (auto el : other) {
                if (!this->includes(el)) {
                    return false;
                }
            }
            return true;
        }

        bool intersects(MeasurementTypeList other) {
            for(Type t : other) {
                if (this->includes(t)) {
                    return true;
                }
            }
            return false;
        }
    };

    class Measurement {
        public:
        virtual ~Measurement() = default;
        virtual MeasurementTypeList getMeasurementTypes() = 0;

        bool supportsMeasurementType(Measurements::Type type) {
            for (Measurements::Type t : this->getMeasurementTypes()) {
                if (t == type) {
                    return true;
                }
            }
            return false;
        }
    };

    class Temperature : public Measurement {
        public:
        virtual ~Temperature() = default;
        virtual float getTemperature() = 0;
    };

    class Humidity : public Measurement {
        public:
        virtual ~Humidity() = default;
        virtual float getHumidity() = 0;
    };

    class DewPoint : public Measurement {
        public:
        virtual ~DewPoint() = default;
        virtual float getDewPoint() = 0;
    };

    class SwitchPosition : public Measurement {
        public:
        virtual ~SwitchPosition() = default;
        virtual uint8_t getNumberOfPositions() = 0;
        virtual uint8_t getSelectedPosition() = 0;
    };

};

