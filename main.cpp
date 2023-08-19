#include <iostream>
#include <raylib.h>
#include <raymath.h>
#define PI 3.14159265

struct PolarVec
{
    float r;
    float a;
};

PolarVec CartToPolar(double x, double y)
{
    float r = sqrt(x*x + y*y);
    float a = (atan(y/x) * 180/PI);
    return (PolarVec) { r, a };
}

Vector2  PolarToCart(float r, float a)
{
    float x = r * cos(a) * PI/180;
    float y = r * sin(a) * PI/180;
    return  (Vector2) { x, y };
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 600;
    
    Rectangle player = { .x = screenWidth/2, .y = screenHeight/2, .width = 50, .height = 50 };
    float angle = 0;
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
        if (IsKeyDown(KEY_RIGHT)) angle += 150 * GetFrameTime();
        if (IsKeyDown(KEY_LEFT)) angle -= 150 * GetFrameTime();
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


        
        
    
        const double x = player.x;
        const double y = player.y;
        //float r = sqrt(x*x + y*y);
        //float a = (atan(y/x) * 180/PI);
        
        //float newPosX = r * cos((a + angle) * PI/180);
        //float newPosY = r * sin((a + angle) * PI/180);
        //float normalisedDirX = newPosX/r;
        //float normalisedDirY = newPosY/r;

        
        //player.x += normalisedDirX;
        //player.y += normalisedDirY;
        Vector2 rV = (Vector2) { (cos(angle * PI/180) - sin(angle * PI/180)),
                                 (cos(angle * PI/180) + sin(angle * PI/180)) };


        float r = sqrt(rV.x*rV.x + rV.y*rV.y);
        float dirX = rV.x/r;
        float dirY = rV.y/r;
        player.x = rV.x + x;
        player.y = rV.y + y;
        
        player.x += (-dirY * vel) * GetFrameTime();
        player.y += (dirX * vel) * GetFrameTime();
        

        DrawRectanglePro(player, (Vector2) { 25, 25 }, angle, RED);
        //DrawRectangle(player.x-25, player.y-25, player.width, player.height, RED);
        //player.x += vel * GetFrameTime();
        DrawLine(player.x, player.y, (dirX * 100) + x,(dirY * 100) + y, GREEN);
        DrawLine(player.x, player.y, (-dirY * 100) + x, (dirX * 100) + y, PURPLE);
        //DrawLine(player.x, player.y, (-dirX * 100) + x, (-dirY * 100) + y, BLACK);
        DrawLine(player.x, player.y, (dirY * 100) + x, (-dirX * 100) + y, BLACK);
        

//        float x = player.x + 25;
//        float y = player.y + 25;
//        PolarVec pol = CartToPolar(x, y);
//        Vector2 cart = PolarToCart(pol.r, pol.a + angle);
//
//        DrawLine(x, y, x - ((cart.x/pol.r) * 100),
//                 y - ((cart.y/pol.r) * 100), GREEN);

        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
 
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}