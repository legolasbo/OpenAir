#ifndef DEPENDENCY_INJECTION_CONTAINER_H
#define DEPENDENCY_INJECTION_CONTAINER_H

#pragma once

#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <cassert>
#include <cstdint>
#include <iostream>

/**
 * Based on https://www.codeproject.com/Articles/1029836/A-Miniature-IOC-Container-in-Cplusplus
*/
class DI {
	static size_t nextTypeId;

public:
	template<typename T>
	static size_t GetTypeId(){
		static size_t typeId = nextTypeId++;
		return typeId;
	}

	DI() : instances() {};
	~DI() { clear(); }

	void clear()
	{
		instances.clear();
	}

	template<typename T>
	void registerInstance(T* instance)
	{
		const int typeId = GetTypeId<T>();
		if (instances.find(typeId) == instances.end())
			instances.emplace(typeId, std::shared_ptr<void>(instance));
	}

	template<typename T>
	std::shared_ptr<T> resolve() const
	{
		const size_t typeId = GetTypeId<T>();
		auto itr1 = instances.find(typeId);
		if (itr1 != instances.end())
			return std::static_pointer_cast<T>(itr1->second);

		return nullptr;
	}

private:
	std::unordered_map<size_t, std::shared_ptr<void>> instances;
};

size_t DI::nextTypeId = 1;

#endif
