#pragma once

#include <map>

template <typename Type>
class Repository {
    private:
    std::map<std::string, std::shared_ptr<Type>> instances;

    protected:
    std::shared_ptr<DI> container;

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
    Repository() {
        this->container = DI::GetContainer();
    }

    void destroyInstances() {
        this->instances.clear();
    }
};

