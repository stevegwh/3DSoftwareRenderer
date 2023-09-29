#pragma once
#include <functional>
#include <utility>

namespace soft3d
{
struct Observer
{
    std::function<void()> callback;
    explicit Observer(std::function<void()> func):callback(std::move(func)){}
};
}

