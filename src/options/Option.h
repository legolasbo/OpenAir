#pragma once

#include <string>
#include <variant>
#include <iostream>

class Option {
    private:
        int i;
        std::string s;

        bool editable = false;

        template <typename T, typename U>
        bool typesMatch() {
            return typeid(T) == typeid(U);
        }

    public:
        enum Type {INTEGER, STRING} type;
        virtual ~Option() = default;
        Option() : Option(0) {};
        Option(std::string v, bool editable = false) {
            this->s = v;
            this->type = STRING;
            this->editable = editable;
        };
        
        Option(int v, bool editable = false) {
            this->i = v;
            this->type = INTEGER;
            this->editable = false;
        }

        bool isEditable() {
            return this->editable;
        }

        virtual Type getType() {
            return this->type;
        }

        int toIntOr(int alternative) {
            if (!this->type == INTEGER) {
                Log.warningln("Reverting to alternative %i", alternative);
                return alternative;
            }
            return this->i;
        }

        int toInt() {
            return this->toIntOr(0);
        }

        std::string toStr() {
            switch (this->type) {
                case INTEGER : return std::to_string(this->toInt());
                case STRING : return this->s;
                default: return "[Option with unknown value]";
            }
        }

        const char * toCharPtr() {
            return this->toStr().c_str();
        }

        virtual Option newValue(String value) {
            switch (this->type) {
                case INTEGER: return Option(value.toInt());
                default: return Option(value.c_str());
            }
        }

        virtual Option newValue(Option value) {
            if (this->type != value.getType()) {
                return *this;
            }

            switch (this->type) {
                case INTEGER: return Option(this->toInt());
                default: return Option(this->toStr());
            }
        }
};

class BoundedOption : public Option {
    private:
        Option lower;
        Option upper;

    public:
        BoundedOption(int value, int lower, int upper) : Option(min(upper, max(lower, value))) {
            this->lower = Option(lower);
            this->upper = Option(upper);
        }

        Option lowerBound() {
            return this->lower;
        }

        Option upperBound() {
            return this->upper;
        }

        Option newValue(Option value) {
            if (this->type != value.getType()) {
                return *this;
            }

            switch(this->type) {
                case INTEGER: return BoundedOption(value.toInt(), this->lower.toInt(), this->upper.toInt());
                default: return *this;
            }
        }

        BoundedOption newValue(int value) {
            return BoundedOption(value, this->lower.toInt(), this->upper.toInt());
        }

};
