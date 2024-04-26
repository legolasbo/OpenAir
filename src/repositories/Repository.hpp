#pragma once

#include <map>
#include <vector>

template <typename Type>
class Repository {
    private:
    std::map<std::string, std::shared_ptr<Type>> instances;

    protected:

    void registerInstance(std::string uuid, std::shared_ptr<Type> instance) {
        if (this->getInstance(uuid) != nullptr) {
            return;
        }
        this->instances.emplace(uuid, instance);
    }


    public:
    virtual ~Repository() = default;
    virtual std::shared_ptr<Type> create(std::string type) = 0;

    virtual void loadJson(JsonObject definitions) {
        for (JsonPair p : definitions) {
            std::string uuid = p.key().c_str();
            if (!p.value().containsKey("type")) {
                Log.warningln("Unable to load %s from json. No type information available for %s", typeid(*this).name(), uuid.c_str());
                continue;
            }
            auto instance = this->create(definitions[p.key()]["type"]);
            instance->configureFromJson(p.value());
            this->registerInstance(uuid, instance);
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

    std::shared_ptr<Type> getInstance(Option opt) {
        return this->getInstance(opt.toCharPtr());
    }

    std::shared_ptr<Type> getInstance(std::string uuid) {
        for ( auto instance : this->instances) {
            if (instance.first == uuid) {
                return instance.second;
            }
        }
        return nullptr;
    }

    void remove(std::string uuid) {
        instances.erase(uuid);
    }
};

