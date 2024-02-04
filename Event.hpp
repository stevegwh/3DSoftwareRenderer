//
// Created by Steve Wheeler on 29/09/2023.
//

#pragma once

#include <functional>
#include <vector>
#include "Observer.hpp"

namespace soft3d
{

class Event
{
private:
    //std::vector<Observer*> observerList{};
    std::vector<std::shared_ptr<Observer>> observerList{};

public:
    void Subscribe(const std::shared_ptr<Observer>& observer);
    void Unsubscribe(const std::shared_ptr<Observer>& observer);
    void InvokeAllEvents() const;
};
}

