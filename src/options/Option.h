#pragma once

#include <string>
#include <variant>
#include <iostream>

class Option {
    private:
        int i;
        std::string s;
        ConnectionType ct;
        SensorConnector sc;

        std::string label;

        bool editable = false;

        template <typename T, typename U>
        bool typesMatch() {
            return typeid(T) == typeid(U);
        }

    public:
        enum Type {INTEGER, STRING, CONNECTOR, CONNECTION} type;
        virtual ~Option() = default;
        Option() : Option(0) {};
        Option(std::string v, std::string label = "", bool editable = false) {
            this->s = v;
            this->type = STRING;
            this->editable = editable;

            this->sc = SensorConnectorFromMachineName(v.c_str());
            this->ct = ConnectionTypeFromMachineName(v.c_str());
            if (this->sc != UNKNOWN_CONNECTOR) {
                this->type = CONNECTOR;
            }
            if (this->ct != UNKNOWN_CONNECTION_TYPE) {
                this->type = CONNECTION;
            }
            this->label = label;
        };
        
        Option(int v, std::string label = "", bool editable = false) {
            this->i = v;
            this->type = INTEGER;
            this->editable = editable;
            this->label = label;
        }

        Option(ConnectionType v, std::string label = "", bool editable = false) {
            this->ct = v;
            this->type = CONNECTION;
            this->editable = editable;
            this->label = label;
        }

        Option(SensorConnector v, std::string label = "", bool editable = false) {
            this->sc = v;
            this->type = CONNECTOR;
            this->editable = editable;
            this->label = label;
        }

        bool isEditable() {
            return this->editable;
        }

        std::string getLabel() {
            return this->label;
        }

        virtual Type getType() {
            return this->type;
        }

        virtual JsonDocument toInterfaceOption() {
            Log.traceln("Option::toInterfaceOption from %s label: %s", typeid(*this).name(), this->getLabel().c_str());
            JsonDocument doc;

            switch (this->type) {
                case INTEGER:
                    doc["type"] = "number";
                    break;
                case STRING: 
                    doc["type"] = "text";
                    break;
                case CONNECTOR:
                case CONNECTION:
                    doc["type"] = "select";
                    break;
            }

            if (!this->isEditable()) {
                doc["type"] = "hidden";
            }
            doc["label"] = this->label;
            doc["value"] = this->toStr();

            return doc;
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

        SensorConnector toConnector() {
            return this->sc;
        }

        ConnectionType toConnection() {
            return this->ct;
        }

        std::string toStr() {
            switch (this->type) {
                case INTEGER : return std::to_string(this->toInt());
                case STRING : return this->s;
                case CONNECTOR: return ToMachineName(this->sc);
                case CONNECTION: return ToMachineName(this->ct);
                default: return "[Option with unknown value]";
            }
        }

        const char * toCharPtr() {
            return this->toStr().c_str();
        }

        virtual Option newValue(ConnectionType value) {
            return Option(value);
        }

        virtual Option NewValue(SensorConnector value) {
            return Option(value);
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

class ListOption : public Option {
    private:
        std::vector<Option> options;

    public:
        template <typename T>
        ListOption(T value, std::vector<Option> options, std::string label = "", bool editable = false) : Option(value, label, editable) {
            this->options = options;
        }
};

class BoundedOption : public Option {
    private:
        Option lower;
        Option upper;

    public:
        BoundedOption(int value, int lower, int upper, std::string label = "", bool editable = false) : Option(min(upper, max(lower, value)), label, editable) {
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

        virtual JsonDocument toInterfaceOption() {
            Log.traceln("Option::toInterfaceOption from %s label: %s", typeid(*this).name(), this->getLabel().c_str());
            auto doc = Option::toInterfaceOption();

            doc["constrain"]["min"] = this->lower.toInt();
            doc["constrain"]["max"] = this->upper.toInt();

            return doc;
        }
};
