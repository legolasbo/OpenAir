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

    class Measurement {
        public:
        virtual std::vector<Type> getMeasurementTypes();
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
