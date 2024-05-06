#pragma once

#include <string>
#include <variant>
#include <iostream>

class Option {
    private:
        int i = 0;
        std::string s = "";
        ConnectionType ct = UNKNOWN_CONNECTION_TYPE;
        SensorConnector sc = UNKNOWN_CONNECTOR;

        std::string label = "";

        bool editable = false;

        template <typename T, typename U>
        bool typesMatch() {
            return typeid(T) == typeid(U);
        }

    public:
        enum Type {INTEGER, STRING, CONNECTOR, CONNECTION} type;
        virtual ~Option() = default;
        Option() : Option(0) {};
        Option(const std::string & v, const std::string &label = "", bool editable = false) {
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
        
        Option(int v, const std::string &label = "", bool editable = false) {
            this->i = v;
            this->type = INTEGER;
            this->editable = editable;
            this->label = label;
        }

        Option(ConnectionType v, const std::string &label = "", bool editable = false) {
            this->ct = v;
            this->type = CONNECTION;
            this->editable = editable;
            this->label = label;
        }

        Option(SensorConnector v, const std::string &label = "", bool editable = false) {
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

        template <typename T>
        std::shared_ptr<Option> newValue(T value) {
            return this->newValue(Option(value));
        }

        virtual std::shared_ptr<Option> newValue(Option value) {
            if (this->type != value.getType()) {
                return this->newValue(*this);
            }

            switch (this->type) {
                case INTEGER: return std::make_shared<Option>(value.toInt(), this->getLabel(), this->isEditable());
                default: return std::make_shared<Option>(value.toStr(), this->getLabel(), this->isEditable());
            }
        }
};

class ListOption : public Option {
    private:
        std::vector<Option> options;

    public:
        template <typename T>
        ListOption(T value, const std::vector<Option> &options, const std::string &label = "", bool editable = false) : Option(value, label, editable) {
            this->options = options;
        }

        std::shared_ptr<Option> newValue(Option value) override {
            if (this->type != value.getType()) {
                return this->newValue(*this);
            }

            for (Option opt : this->options) {
                if (value.toStr() == opt.toStr()) {
                    switch (this->type) {
                        case INTEGER:
                            return std::make_shared<ListOption>(value.toInt(), this->options, this->getLabel(), this->isEditable());
                        case CONNECTION:
                            return std::make_shared<ListOption>(value.toConnection(), this->options, this->getLabel(), this->isEditable());
                        case CONNECTOR:
                            return std::make_shared<ListOption>(value.toConnector(), this->options, this->getLabel(), this->isEditable());
                        default:
                            return std::make_shared<ListOption>(value.toStr(), this->options, this->getLabel(), this->isEditable());
                    }
                }
            }
            
            return this->newValue(*this);
        }

        JsonDocument toInterfaceOption() override {
            auto doc = Option::toInterfaceOption();

            doc["type"] = "select";
            for (auto option : this->options) {
                std::string label = option.getLabel();
                if (label == "") {
                    label = option.toStr();
                }
                doc["options"][option.toStr()] = label;
            }

            return doc;
        }
};

class BoundedOption : public Option {
    private:
        Option lower;
        Option upper;

    public:
        BoundedOption(int value, int lower, int upper, const std::string &label = "", bool editable = false) : Option(min(upper, max(lower, value)), label, editable) {
            this->lower = Option(lower);
            this->upper = Option(upper);
        }

        Option lowerBound() {
            return this->lower;
        }

        Option upperBound() {
            return this->upper;
        }

        std::shared_ptr<Option> newValue(Option value) override {
            if (this->type != value.getType()) {
                return this->newValue(*this);
            }

            switch(this->type) {
                case INTEGER: return std::make_shared<BoundedOption>(value.toInt(), this->lower.toInt(), this->upper.toInt());
                default: return this->newValue(*this);
            }
        }

        JsonDocument toInterfaceOption() override {
            auto doc = Option::toInterfaceOption();

            doc["constrain"]["min"] = this->lower.toInt();
            doc["constrain"]["max"] = this->upper.toInt();

            return doc;
        }
};
