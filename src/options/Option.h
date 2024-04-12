#pragma once

#include <string>
#include <variant>
#include <iostream>

class Option {
    private:
        union Value {
            int i;
            const char * s;
        };

        enum Type {INTEGER, STRING} type;
        Value _value;

        template <typename T, typename U>
        bool typesMatch() {
            return typeid(T) == typeid(U);
        }

    public:
        virtual ~Option() = default;
        Option() : Option(0) {};
        Option(std::string v) : Option(v.c_str()) {};
        Option(const char * v) {
            this->_value.s = v;
            this->type = STRING;
        }

        Option(int v) {
            this->_value.i = v;
            this->type = INTEGER;
        }

        virtual Type getType() {
            return this->type;
        }

        int toIntOr(int alternative) {
            if (!this->type == INTEGER) {
                return alternative;
            }
            return this->_value.i;
        }

        int toInt() {
            return this->toIntOr(0);
        }

        const char * toStr() {
            switch (this->type) {
                case INTEGER : return std::to_string(this->toInt()).c_str();
                case STRING : return this->_value.s;
                default: return "[Option with unknown value]";
            }
        }
};

class BoundedOption : public Option {
    private:
        Option lower;
        Option upper;

    public:
        BoundedOption(int value, int lower, int upper) : Option(value) {
            this->lower = Option(lower);
            this->upper = Option(upper);
        }

        Option lowerBound() {
            return this->lower;
        }

        Option upperBound() {
            return this->upper;
        }

};
