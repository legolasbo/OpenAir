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

std::shared_ptr<Option> createOption(int value, std::string label = "", bool editable = true);
std::shared_ptr<Option> createOption(const std::string &value, std::string label = "", bool editable = true);
std::shared_ptr<Option> createOption(ConnectionType value, std::string label = "", bool editable = true);
std::shared_ptr<Option> createOption(SensorConnector value, std::string label = "", bool editable = true);

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

    return std::make_shared<StringOption>(value, label, editable);
}
std::shared_ptr<Option> createOption(ConnectionType value, std::string label, bool editable) {
    return std::make_shared<ConnectionTypeOption>(value, label, editable);
}
std::shared_ptr<Option> createOption(SensorConnector value, std::string label, bool editable) {
    return std::make_shared<SensorConnectorOption>(value, label, editable);
}