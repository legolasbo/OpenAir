#ifndef FACTORY_H
#define FACTORY_H

#include <map>

template <typename Type>
class Factory {
    private:
    std::map<std::string, Type*> instances;

    protected:
    void registerInstance(std::string uuid, Type * instance) {
        if (this->getInstance(uuid) != nullptr) {
            return;
        }
        this->instances.insert(std::pair<std::string, Type *>(uuid, instance));
    }

    Type * getInstance(std::string uuid) {
        for ( auto instance : this->instances) {
            if (instance.first == uuid) {
                return instance.second;
            }
        }
        return nullptr;
    }

    public:
    void destroyInstances() {
        for (auto instance : this->instances) {
            delete(instance.second);
        }
        this->instances.clear();
    }
};

#endif
