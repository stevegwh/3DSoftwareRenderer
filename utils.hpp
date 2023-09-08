//
// Created by Steve Wheeler on 25/08/2023.
//

#pragma once
#include <cstdint>
#include <SDL2/SDL.h>
#include "lodepng.h"

#define FPS_INTERVAL 1.0


struct FPSCounter
{
    Uint32 fps_lasttime; //the last recorded time.
    Uint32 fps_current; //the current FPS.
    Uint32 fps_frames;
    
    FPSCounter()
    {
        fps_lasttime = SDL_GetTicks();
        fps_frames = 0;
        fps_current = 0;
    }
    
    void Update()
    {
        fps_frames++;
        if (fps_lasttime < SDL_GetTicks() - FPS_INTERVAL*1000)
        {
            fps_lasttime = SDL_GetTicks();
            fps_current = fps_frames;
            fps_frames = 0;
        }
    }

};
struct Clock
{
    uint32_t last_tick_time = 0;
    uint32_t delta = 0;

    void tick()
    {
        uint32_t tick_time = SDL_GetTicks();
        delta = tick_time - last_tick_time;
        last_tick_time = tick_time;
    }
};


