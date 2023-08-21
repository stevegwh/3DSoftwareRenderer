#include <iostream>
#include <SDL2/SDL.h>
#include "stevelib.h"
#include <vector>
#include "constants.h"

void DrawLine(SDL_Renderer& renderer, float x1, float y1, float x2, float y2)
{
    
}

int main(int argc, char *argv[])
{
    float angle = 0;
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);


    SDL_bool loop = SDL_TRUE;
    SDL_Event event;
    Color col = { 0, 0, 0 };

    while (loop)
    {
        // Allow quiting with escape key by polling for pending events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                loop = SDL_FALSE;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        loop = SDL_FALSE;
                        break;
                    case SDLK_LEFT:
                        angle -= 5;
                        break;
                    case SDLK_RIGHT:
                        angle += 5;
                        break;
                    default:
                        loop = SDL_TRUE;
                }
            }
        }
        // Update
        SDL_Rect rect = { 50, 50, 50, 50 };

//        std::vector<Vector3> points = {
//            (Vector3) {-50, 50, -50},
//            (Vector3) { 50, 50, -50 },
//            (Vector3) { -50, -50, -50 },
//            (Vector3) { 50, -50, -50 },
//            (Vector3) { -50, 50, 50 },
//            (Vector3) { 50, 50, 50 },
//            (Vector3) { -50, -50, 50 },
//            (Vector3) { 50, -50, 50 }
//        };

        std::vector<Vector3> points = {
            (Vector3) {-50 + SCREEN_WIDTH/2, 50 + SCREEN_HEIGHT/2, -50},
            (Vector3) { 50 + SCREEN_WIDTH/2, 50 + SCREEN_HEIGHT/2, -50 },
            (Vector3) { -50 + SCREEN_WIDTH/2, -50 + SCREEN_HEIGHT/2, -50 },
            (Vector3) { 50 + SCREEN_WIDTH/2, -50 + SCREEN_HEIGHT/2, -50 },
            (Vector3) { -50 + SCREEN_WIDTH/2, 50 + SCREEN_HEIGHT/2, 50 },
            (Vector3) { 50 + SCREEN_WIDTH/2, 50 + SCREEN_HEIGHT/2, 50 },
            (Vector3) { -50 + SCREEN_WIDTH/2, -50 + SCREEN_HEIGHT/2, 50 },
            (Vector3) { 50 + SCREEN_WIDTH/2, -50 + SCREEN_HEIGHT/2, 50 }
        };

        // Draw
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (auto v : points)
        {
//            SDL_Rect r = { (int)v.x, (int)v.y, 3, 3 };
//            SDL_RenderDrawRect(renderer, &r);
//            SDL_RenderFillRect(renderer, &r);
            SDL_RenderDrawPointF(renderer, v.x, v.y);
        }
//        SDL_RenderDrawRect(renderer, &rect);
//        SDL_RenderFillRect(renderer, &rect);
        SDL_RenderPresent(renderer);
    }


    return 0;
}