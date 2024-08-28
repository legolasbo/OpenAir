#pragma once

template <typename Type>
class RepositorySubscriber {
    public:
        virtual void instanceAdded(std::shared_ptr<Type> instance) = 0;
        virtual void instanceRemoved(std::shared_ptr<Type> instance) = 0;
};
