#pragma once

#include <set>

namespace Measurements {
    typedef std::function<int(void)> MeasurementCallback;

    enum Type {
        UNKNOWN,
        TemperatureMeasurement,
        HumidityMeasurement,
        DewPointMeasurement,
        PressureMeasurement,
        SwitchPositionMeasurement,
        SwitchPositionCountMeasurement,
        CO2Measurement,
    };


    const char* ToString(Type t) {
        switch (t)
        {
            case CO2Measurement:                    return "Co2";
            case DewPointMeasurement:               return "Dew point";
            case HumidityMeasurement:               return "Humidity";
            case PressureMeasurement:               return "Pressure";
            case SwitchPositionMeasurement:         return "Switch position";
            case SwitchPositionCountMeasurement:    return "Switch position count";
            case TemperatureMeasurement:            return "Temperature";
            default:                                return "[Unknown measurement type]";
        }
    }

    const char* ToMachineName(Type t) {
        switch (t) {
            case CO2Measurement:                    return "measurement_co2";
            case DewPointMeasurement:               return "measurement_dew-point";
            case HumidityMeasurement:               return "measurement_humidity";
            case PressureMeasurement:               return "measurement_pressure";
            case SwitchPositionMeasurement:         return "measurement_switch-position";
            case SwitchPositionCountMeasurement:    return "measurement_switch-position-count";
            case TemperatureMeasurement:            return "measurement_temperature";
            default:                                return "measurement_unknown";
        }
    }

    const Type FromMachineName(const char* name) {
        if (strcmp(name, ToMachineName(CO2Measurement)) == 0) {
            return CO2Measurement;
        }
        if (strcmp(name, ToMachineName(DewPointMeasurement)) == 0) {
            return DewPointMeasurement;
        }
        if (strcmp(name, ToMachineName(HumidityMeasurement)) == 0) {
            return HumidityMeasurement;
        }
        if (strcmp(name, ToMachineName(PressureMeasurement)) == 0) {
            return PressureMeasurement;
        }
        if (strcmp(name, ToMachineName(SwitchPositionMeasurement)) == 0) {
            return SwitchPositionMeasurement;
        }
        if (strcmp(name, ToMachineName(SwitchPositionCountMeasurement)) == 0) {
            return SwitchPositionCountMeasurement;
        }
        if (strcmp(name, ToMachineName(TemperatureMeasurement)) == 0) {
            return TemperatureMeasurement;
        }
        return UNKNOWN;
    }

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

