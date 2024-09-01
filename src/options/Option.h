#pragma once

#include <string>
#include <variant>
#include <iostream>
#include "../enums/ConnectionTypes.h"
#include "../enums/SensorConnectors.h"
#include "../enums/SensorTypes.h"

class Option {
    private:
        std::string label;
        bool editable;

    public:
        virtual ~Option() {};
        Option(const std::string &label, bool editable) : label(label), editable(editable) {};

        bool isEditable() {
            return this->editable;
        }

        const std::string &getLabel() {
            return this->label;
        }

        virtual JsonDocument toInterfaceOption() {
            JsonDocument doc;

            if (!this->isEditable()) {
                doc["type"] = "hidden";
            }
            doc["label"] = this->label;
            doc["value"] = this->toStr();

            return doc;
        }

        virtual std::string typeName() = 0;
        virtual std::string toStr() = 0;
        virtual std::shared_ptr<Option> newValue(const std::string &value) = 0;

        template <typename T>
        T* as() {            
            return dynamic_cast<T*>(this);
        }


};

class MultiValueOption {
    public:
        virtual std::shared_ptr<Option> newValues(const std::map<std::string, std::string> &values) = 0;
        virtual JsonDocument toJson() = 0;
};

std::shared_ptr<Option> createOption(int value, std::string label = "", bool editable = true);
std::shared_ptr<Option> createOption(const std::string &value, std::string label = "", bool editable = true);
std::shared_ptr<Option> createOption(ConnectionType value, std::string label = "", bool editable = true);
std::shared_ptr<Option> createOption(SensorConnector value, std::string label = "", bool editable = true);
std::shared_ptr<Option> createOption(SensorType value, std::string label = "Sensor type", bool editable = true);
std::shared_ptr<Option> createOption(Measurements::Type value, std::string label = "Measurement type", bool editable = true);

class UnknownOption : public Option {
    public:
        UnknownOption() : Option("UNKNOWN", false) {}

        std::shared_ptr<Option> newValue(const std::string &value) override {
            return std::make_shared<UnknownOption>();
        }

        std::string toStr() override {
            return this->getLabel();
        }

        std::string typeName() override {
            return typeid(*this).name();
        }
};

class StringOption : public Option {
    private:
        std::string value;

    public:
        explicit StringOption(const std::string &value, const std::string &label, bool editable) : Option(label, editable), value(value) {}

        std::string toStr() override {
            return this->value;
        }

        const std::string &getValue() {
            return this->value;
        }

        std::shared_ptr<Option> newValue(const std::string &value) override {
            return createOption(value, this->getLabel(), this->isEditable());
        }

        std::string typeName() override {
            return typeid(value).name();
        }

        JsonDocument toInterfaceOption() override {
            auto doc = Option::toInterfaceOption();

            doc["type"] = "text";

            return doc;
        }
};

class IntegerOption : public Option {
    private:
        int value;

    public:
        explicit IntegerOption(int value, const std::string &label, bool editable) : Option(label, editable), value(value) {}

        std::string toStr() override {
            return std::to_string(this->value);
        }

        int getValue() {
            return value;
        }

        std::shared_ptr<Option> newValue(const std::string &value) override {
            int val = atoi(value.c_str());
            return createOption(val, this->getLabel(), this->isEditable());
        }

        std::string typeName() override {
            return typeid(value).name();
        }

        JsonDocument toInterfaceOption() override {
            auto doc = Option::toInterfaceOption();

            doc["type"] = "number";

            return doc;
        }

};

class BooleanOption : public Option {
    private:
        bool value;

    public:
        explicit BooleanOption(bool value, const std::string &label, bool editable) : Option(label, editable), value(value) {}

        std::string toStr() override {
            return std::to_string(this->value);
        }

        bool getValue() {
            return value;
        }

        std::shared_ptr<Option> newValue(const std::string &value) override {
            int val = atoi(value.c_str());
            return std::make_shared<BooleanOption>(val != 0, this->getLabel(), this->isEditable());
        }

        std::string typeName() override {
            return typeid(value).name();
        }

        JsonDocument toInterfaceOption() override {
            auto doc = Option::toInterfaceOption();

            doc["type"] = "boolean";

            return doc;
        }

};

class SensorTypeOption : public Option {
    private:
        SensorType value;
    
    public:
        explicit SensorTypeOption(SensorType value, const std::string &label, bool editable) : Option(label, editable), value(value) {}

        std::string toStr() override {
            return ToMachineName(this->value);
        }

        SensorType getValue() {
            return this->value;
        }

        std::shared_ptr<Option> newValue(const std::string &value) override {
            return createOption(SensorTypeFromMachineName(value.c_str()), this->getLabel(), this->isEditable());
        }

        std::string typeName() override {
            return typeid(value).name();
        }
};

class MeasurementTypeOption : public Option {
    private:
        Measurements::Type value;
    
    public:
        explicit MeasurementTypeOption(Measurements::Type value, const std::string &label, bool editable) : Option(label, editable), value(value) {}

        std::string toStr() override {
            return ToMachineName(this->value);
        }

        Measurements::Type getValue() {
            return this->value;
        }

        std::shared_ptr<Option> newValue(const std::string &value) override {
            return createOption(Measurements::FromMachineName(value.c_str()), this->getLabel(), this->isEditable());
        }

        std::string typeName() override {
            return typeid(value).name();
        }
};

class ConnectionTypeOption : public Option {
    private:
        ConnectionType value;

    public:
        explicit ConnectionTypeOption(ConnectionType value, const std::string &label, bool editable) : Option(label, editable), value(value) {}

        std::string toStr() override {
            return ToString(this->value);
        }

        ConnectionType getValue() {
            return this->value;
        }

        std::shared_ptr<Option> newValue(const std::string &value) override {
            return createOption(ConnectionTypeFromMachineName(value.c_str()), this->getLabel(), this->isEditable());
        }

        std::string typeName() override {
            return typeid(value).name();
        }

};

class SensorConnectorOption : public Option {
    private:
        SensorConnector value;

    public:
        explicit SensorConnectorOption(SensorConnector value, const std::string &label, bool editable) : Option(label, editable), value(value) {}

        std::string toStr() override {
            return ToString(this->value);
        }

        SensorConnector getValue() {
            return value;
        }

        std::shared_ptr<Option> newValue(const std::string &value) override {
            return createOption(SensorConnectorFromMachineName(value.c_str()), this->getLabel(), this->isEditable());
        }

        std::string typeName() override {
            return typeid(value).name();
        }

};

template <typename T>
class ListOption : public Option {
    private:
        std::vector<std::shared_ptr<Option>> options;
        T value;

        template <typename Y>
        std::shared_ptr<Option> create(Y value) {
            return std::make_shared<ListOption<Y>>(value, this->options, this->getLabel(), this->isEditable());
        }

    public:
        explicit ListOption(T value, const std::vector<std::shared_ptr<Option>> &options, const std::string &label, bool editable) : Option(label, editable) {
            bool foundValue = false;

            std::vector<std::shared_ptr<Option>> opts;

            for(auto o : options) {
                if (o->typeName() == typeid(T).name()) {
                    opts.push_back(o);
                }
            }
            this->options = opts;
            this->value = value;
        }

        std::shared_ptr<Option> newValue(const std::string &value) override {
            auto opt = createOption(value);
            return this->newValue(opt);
        }

        std::shared_ptr<Option> newValue(std::shared_ptr<Option> opt) {
            Option* optptr = opt.get();
            if (auto o = dynamic_cast<StringOption*>(optptr)) {
                return this->create(o->getValue());
            }
            if (auto o = dynamic_cast<IntegerOption*>(optptr)) {
                return this->create(o->getValue());
            }
            if (auto o = dynamic_cast<ConnectionTypeOption*>(optptr)) {
                return this->create(o->getValue());
            }
            if (auto o = dynamic_cast<SensorConnectorOption*>(optptr)) {
                return this->create(o->getValue());
            }
            if (auto o = dynamic_cast<MeasurementTypeOption*>(optptr)) {
                return this->create(o->getValue());
            }
            if (auto o = dynamic_cast<SensorTypeOption*>(optptr)) {
                return this->create(o->getValue());
            }

            return this->create(this->value);
        }

        std::string toStr() override {
            return createOption(value)->toStr();
        }

        T getValue() {
            return this->value;
        }

        JsonDocument toInterfaceOption() override {
            auto doc = Option::toInterfaceOption();

            doc["type"] = "select";
            for (auto option : this->options) {
                std::string label = option->getLabel();
                if (label == "") {
                    label = option->toStr();
                }
                doc["options"][option->toStr()] = label;
            }

            return doc;
        }

        std::string typeName() override {
            return typeid(*this).name();
        }

};

class BoundedOption : public IntegerOption {
    private:
        int lower;
        int upper;

    public:
        explicit BoundedOption(int value, int lower, int upper, const std::string &label = "", bool editable = false) : IntegerOption(min(upper, max(lower, value)), label, editable) {
            this->lower = lower;
            this->upper = upper;
        }

        int lowerBound() {
            return this->lower;
        }

        int upperBound() {
            return this->upper;
        }

        JsonDocument toInterfaceOption() override {
            auto doc = IntegerOption::toInterfaceOption();

            doc["constrain"]["min"] = this->lower;
            doc["constrain"]["max"] = this->upper;

            return doc;
        }

        std::shared_ptr<Option> newValue(const std::string &value) override {
            return std::make_shared<BoundedOption>(atoi(value.c_str()), this->lower, this->upper, this->getLabel(), this->isEditable());
        }

        std::string typeName() override {
            return typeid(this->getValue()).name();
        }
        
};

class RangeOption : public Option, public MultiValueOption {
    private:
        int _lowerBound, _lower, _upper, _upperBound;

    public:
        explicit RangeOption(int lowerBound, int lower, int upper, int upperBound, const std::string &label = "") : Option(label, true) {
            this->_lowerBound = min(lowerBound, upperBound);
            this->_upperBound = max(lowerBound, upperBound);
            this->_lower = max(lowerBound, lower);
            this->_upper = min(upperBound, upper);
        }

        int lowerBound() {
            return this->_lowerBound;
        }

        int upperBound() {
            return this->_upperBound;
        }

        int lower() {
            return this->_lower;
        }

        int upper () {
            return this->_upper;
        }

        
        JsonDocument toInterfaceOption() override {
            JsonDocument doc = Option::toInterfaceOption();

            doc["type"] = "range";
            doc.remove("value");
            doc["lowerBound"] = this->_lowerBound;
            doc["lower"] = this->_lower;
            doc["upper"] = this->_upper;
            doc["upperBound"] = this->_upperBound;

            return doc;
        }

        std::shared_ptr<Option> newValues(const std::map<std::string, std::string> &values) override {
            int lower = this->_lower;
            int upper = this->_upper;

            for (auto i : values) {
                if (i.first == "lower") {
                    lower = atoi(i.second.c_str());
                    continue;
                }
                if (i.first == "upper") {
                    upper = atoi(i.second.c_str());
                }
            }

            return std::make_shared<RangeOption>(this->_lowerBound, lower, upper, this->_upperBound, this->getLabel());
        }

        std::shared_ptr<Option> newValue(const std::string &value) override {
            return std::make_shared<RangeOption>(this->_lowerBound, this->_lower, this->_upper, this->_upperBound, this->getLabel());
        }

        std::string typeName() override {
            return "range";
        }

        std::string toStr() override {
            char out[100];
            sprintf(out, "Range: %d -> %d", this->_lower, this->_upper);
            return out;
        }

        JsonDocument toJson() override {
            JsonDocument doc;

            doc["lower"] = this->_lower;
            doc["upper"] = this->_upper;

            return doc;
        }
        
};


std::shared_ptr<Option> createOption(int value, std::string label, bool editable) {
    return std::make_shared<IntegerOption>(value, label, editable);
}
std::shared_ptr<Option> createOption(const std::string &value, std::string label, bool editable) {
    auto ct = ConnectionTypeFromMachineName(value.c_str());
    if (ct != UNKNOWN_CONNECTION_TYPE) {
        return createOption(ct, label, editable);
    }

    auto conn = SensorConnectorFromMachineName(value.c_str());
    if(conn != UNKNOWN_CONNECTOR) {
        return createOption(conn, label, editable);
    }

    auto st = SensorTypeFromMachineName(value.c_str());
    if (st != UNKNOWN_SENSOR_TYPE) {
        return createOption(st, label, editable);
    }

    auto mt = Measurements::FromMachineName(value.c_str());
    if (mt != Measurements::Type::UNKNOWN) {
        return createOption(mt, label, editable);
    }

    return std::make_shared<StringOption>(value, label, editable);
}
std::shared_ptr<Option> createOption(ConnectionType value, std::string label, bool editable) {
    return std::make_shared<ConnectionTypeOption>(value, label, editable);
}
std::shared_ptr<Option> createOption(SensorConnector value, std::string label, bool editable) {
    return std::make_shared<SensorConnectorOption>(value, label, editable);
}
std::shared_ptr<Option> createOption(SensorType value, std::string label, bool editable) {
    return std::make_shared<SensorTypeOption>(value, label, editable);
}
std::shared_ptr<Option> createOption(Measurements::Type value, std::string label, bool editable) {
    return std::make_shared<MeasurementTypeOption>(value, label, editable);
}
