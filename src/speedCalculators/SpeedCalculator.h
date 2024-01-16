#include <string>

class SpeedCalculator {
    public:
        void print();
        virtual int calculate();
        virtual std::string name();
};
