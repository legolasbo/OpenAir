#pragma once

#include <unordered_map>
#include <memory>

/**
 * Loosely based on https://www.codeproject.com/Articles/1029836/A-Miniature-IOC-Container-in-Cplusplus
*/
class DI {
private:
	DI() : instances() {};
	static std::shared_ptr<DI> container;

public:
	DI(DI &other) = delete;
	void operator=(const DI &) = delete;
	static std::shared_ptr<DI> GetContainer();

	~DI() { clear(); }

	void clear() {
		instances.clear();
	}

	template<typename T>
	void registerInstance(std::shared_ptr<T> instance) {
		const size_t typeId = typeid(T).hash_code();
		if (instances.find(typeId) == instances.end())
			instances.emplace(typeId, instance);
	}

	template<typename T>
	void registerInstance(T &instance) {
		this->registerInstance(std::shared_ptr<T>(&instance));
	}

	template<typename T>
	std::shared_ptr<T> resolve() {
		const size_t typeId = typeid(T).hash_code();
		auto itr1 = instances.find(typeId);
		if (itr1 != instances.end())
			return std::static_pointer_cast<T>(itr1->second);

		std::shared_ptr<T> instance(new T());
		this->registerInstance<T>(instance);
		return instance;
	}


private:
	std::unordered_map<size_t, std::shared_ptr<void>> instances;
};

std::shared_ptr<DI> DI::container = nullptr;

std::shared_ptr<DI> DI::GetContainer() {
	if (DI::container == nullptr) {
		DI::container = std::shared_ptr<DI>(new DI());
	}
	return DI::container;
}
