//
// Created by Steve Wheeler on 29/09/2023.
//

#include "Event.hpp"

namespace soft3d
{

void Event::InvokeAllEvents() const
{
    for (Observer* ob : observerList)
    {
        ob->callback();
    }
}

void Event::Subscribe(Observer* observer)
{
    observerList.push_back(observer);
}

void Event::Unsubscribe(Observer* observer)
{
    for (auto it = observerList.begin(); it != observerList.end(); ++it)
    {
        if (*it == observer)
        {
            observerList.erase(it);
            delete *it;
            break;
        }
    }
}

Event::Event() = default;

Event::~Event()
{
    for ( auto& observer : observerList) 
    {
        delete observer;
    }
}

}