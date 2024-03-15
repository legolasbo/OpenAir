#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H

#include <vector>
namespace Measurements {

    enum Type {
        TemperatureMeasurement,
        HumidityMeasurement,
        DewPointMeasurement,
        PressureMeasurement,
        SwitchPositionMeasurement,
    };

    class MeasurementTypeList : public std::vector<Type> {
        public:
        MeasurementTypeList() : std::vector<Type>(){}
        MeasurementTypeList(std::initializer_list<Type> s) : std::vector<Type>(s) {}
        
        bool includes(Type needle) {
            for (size_t i = 0; i < this->size(); i++) {
                if (this->at(i) == needle) {
                    return true;
                }
            }
            return false;
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
        virtual MeasurementTypeList getMeasurementTypes();

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
        virtual float getTemperature();
    };

    class Humidity : public Measurement {
        public:
        virtual float getHumidity();
    };

    class DewPoint : public Measurement {
        public:
        virtual float getDewPoint();
    };

    class SwitchPosition : public Measurement {
        public:
        virtual uint8_t getNumberOfPositions();
        virtual uint8_t getSelectedPosition();
    };

};

#endif
