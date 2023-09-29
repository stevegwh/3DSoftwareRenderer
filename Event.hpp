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
    std::vector<Observer*> observerList{};

public:
    void Subscribe(Observer* observer);
    void Unsubscribe(Observer* observer);
    void InvokeAllEvents() const;
    Event();
    ~Event();
};
}

