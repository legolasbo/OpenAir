#pragma once

#include <map>
#include <vector>
#include "RepositorySubscriber.hpp"

template <typename Type>
class Repository {
    private:
        std::set<std::shared_ptr<RepositorySubscriber<Type>>> subscribers;

    protected:
    std::map<std::string, std::shared_ptr<Type>> instances;

    void registerInstance(const std::string &uuid, std::shared_ptr<Type> instance) {
        if (this->getInstance(uuid) != nullptr) {
            return;
        }
        this->instances.emplace(uuid, instance);

        for (auto s : this->subscribers) {
            s->instanceAdded(instance);
        }
    }


    public:
    virtual ~Repository() = default;
    virtual std::shared_ptr<Type> create(const std::string &type) = 0;

    void registerSubscriber(std::shared_ptr<RepositorySubscriber<Type>> s) {
        this->subscribers.emplace(s);
        for (auto i : this->instances) {
            s->instanceAdded(i.second);
        }
    }

    virtual void loadJson(JsonObject definitions) {
        for (JsonPair p : definitions) {
            std::string uuid = p.key().c_str();
            if (!p.value().containsKey("type")) {
                Log.warningln("Unable to load %s from json. No type information available for %s", typeid(*this).name(), uuid.c_str());
                continue;
            }
            auto instance = this->create(definitions[p.key()]["type"]);
            instance->configureFromJson(p.value());
            this->registerInstance(instance->getUuid(), instance);
        }
    };

    virtual JsonDocument toJson() {
        JsonDocument doc;
        
        for ( auto instance : this->instances ) {
            doc[instance.first] = instance.second->toJson();
        }

        return doc;
    };

    std::vector<std::string> getUuids() {
        std::vector<std::string> uuids;

        for ( auto instance : this->instances ) {
            uuids.push_back(instance.first);
        }

        return uuids;
    }

    virtual void addInstance(std::shared_ptr<Type> instance) {
        this->registerInstance(instance->getUuid(), instance);
    }

    std::shared_ptr<Type> getInstance(std::shared_ptr<Option> opt) {
        return this->getInstance(opt->toStr().c_str());
    }

    std::shared_ptr<Type> getInstance(const std::string &uuid) {
        for ( auto instance : this->instances) {
            if (instance.first == uuid) {
                return instance.second;
            }
        }
        return nullptr;
    }

    virtual void remove(const std::string &uuid) {
        auto instance = this->getInstance(uuid);

        instances.erase(uuid);

        for (auto s : this->subscribers) {
            s->instanceRemoved(instance);
        }
    }
};

