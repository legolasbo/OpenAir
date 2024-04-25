#ifndef FACTORY_H
#define FACTORY_H

#include <map>

template <typename Type>
class Factory {
    private:
    std::map<std::string, std::shared_ptr<Type>> instances;

    protected:
    DI &container;

    void registerInstance(std::string uuid, std::shared_ptr<Type> instance) {
        if (this->getInstance(uuid) != nullptr) {
            return;
        }
        this->instances.emplace(uuid, instance);
    }

    std::shared_ptr<Type> getInstance(std::string uuid) {
        for ( auto instance : this->instances) {
            if (instance.first == uuid) {
                return instance.second;
            }
        }
        return nullptr;
    }

    public:
    Factory(DI &container) : container(container) {}

    void destroyInstances() {
        this->instances.clear();
    }
};

#endif
