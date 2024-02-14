
#ifndef GENERIC_CONFIGURATIONS_H
#define GENERIC_CONFIGURATIONS_H


#include <vector>
#include <ArduinoJson.h>

class GenericConfiguration {
    private:

    public:
    GenericConfiguration() {}
    bool isValid() {
        return false;
    }

    virtual bool equals(GenericConfiguration * other) {
        return this->machineName() == other->machineName();
    }

    JsonDocument toJson() {
        JsonDocument doc;
        return doc;
    }

    GenericConfiguration * fromJson() {
        return nullptr;
    }

    virtual const char * machineName() {
        return "";
    }

    virtual std::string editForm() {
        return "Generic config<br>";
    }
};

#endif
