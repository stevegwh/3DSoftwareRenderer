#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include "Triangle.h"
#include "sMesh.hpp"

constexpr int screenWidth = 800;
constexpr int screenHeight = 600;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------

    
    Rectangle player = { .x = screenWidth/2, .y = screenHeight/2, .width = 50, .height = 50 };
    float angle = 1;
    int vel = 0;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------
        if (IsKeyDown(KEY_RIGHT)) angle -= 150 * GetFrameTime();
        if (IsKeyDown(KEY_LEFT)) angle += 150 * GetFrameTime();
        if (IsKeyDown(KEY_A))
        {
            vel = -150;
        }
        else if (IsKeyDown(KEY_D))
        {
            vel = 150;
        }
        else
        {
            vel = 0;
        }


        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);
        float width = 150;
        float height = 150;

        Vector2 p1 = (Vector2) { screenWidth/2 + width/2, screenHeight/2 };
        Vector2 p2 = (Vector2) { screenWidth/2 + width/2, screenHeight/2 + height};
        Vector2 p3 = (Vector2) { p2.x - width, p2.y };
        
        Vector2 pb1 = (Vector2) { p3.x, p3.y };
        Vector2 pb2 = (Vector2) { p3.x, p1.y};
        Vector2 pb3 = (Vector2) { p1.x, p1.y };
        
        Triangle tri(p1, p2, p3);
        Triangle tri2(pb1, pb2, pb3);
        std::vector<Triangle*> tris = { &tri, &tri2 };
        sMesh mesh(tris);
        mesh.Rotate(angle);
        mesh.Draw();
 
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}