#pragma once

#include <set>

namespace Measurements {
    typedef std::function<int(void)> MeasurementCallback;

    enum Type {
        TemperatureMeasurement,
        HumidityMeasurement,
        DewPointMeasurement,
        PressureMeasurement,
        SwitchPositionMeasurement,
        SwitchPositionCountMeasurement,
        CO2Measurement,
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
        protected:
        MeasurementCallback _cb;

        public: 
        Measurement () : _cb(NULL) {}
        Measurement (const MeasurementCallback &cb) : Measurement() {
            this->_cb = cb;
        }

        int measure() {
            if (this->_cb) {
                return this->_cb();
            }

            return 0;
        }
    };

    class MeasurementProvider {
        public:
            virtual MeasurementTypeList getMeasurementTypes() = 0;
            virtual Measurement provide(Measurements::Type measurementType) = 0;

            bool supportsMeasurementType(Measurements::Type type) {
                for (Measurements::Type t : this->getMeasurementTypes()) {
                    if (t == type) {
                        return true;
                    }
                }
                return false;
            }
    };

};

