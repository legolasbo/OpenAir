#pragma once

#include "discovery.h"

class HaSensorProvider {

    public:
        virtual ~HaSensorProvider() {}
        virtual std::set<std::shared_ptr<HaSensor>> getHaSensors() = 0;
};
