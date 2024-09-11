//
// Created by Steve Wheeler on 07/10/2023.
//

#pragma once
#include "constants.hpp"
#include <array>

namespace sage
{
struct ZBuffer
{
    static constexpr unsigned long screenSize = SCREEN_WIDTH * SCREEN_HEIGHT;
    std::array<float, screenSize> buffer{};
    void
    clear()
    {
        std::fill_n(buffer.begin(), screenSize, 0);
    }
};
}
