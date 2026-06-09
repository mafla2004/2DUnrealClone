#include <iostream>
#include <raylib.h>
//#include "ball.h"
#include "basegame.h"

#define GAME_LOOP while (!WindowShouldClose())
#define GAME_NAME "2D Unreal with Ruff :3"

#define DRAW(x) BeginDrawing();\
{x}\
EndDrawing();

constexpr int TargetFramerate = 60;

int main() 
{
    std::cout << "STARTING TEST ON INHERITANCE" << std::endl;

    Resolution res;
    /*if (ConfigFileExists())
    {
        res = GetResolutionFromConfig();
    }
    else
    {
        res = {800, 600};
    }*/
    res = {800, 600};

    InitWindow(res.x, res.y, GAME_NAME);
    SetTargetFPS(TargetFramerate);
    
    GAME_LOOP
    {
        DRAW(
            ClearBackground(BLACK);
            DrawText("Un-Ruff-Eal :3", 190, 200, 20, LIGHTGRAY);
        );
    }
    
    CloseWindow();
    
    /*
    const Color darkGreen = {20, 160, 133, 255};
    
    constexpr int screenWidth = 800;
    constexpr int screenHeight = 600;
    
    Ball ball;
    
    InitWindow(screenWidth, screenHeight, "My first RAYLIB program!");
    SetTargetFPS(60);
    
    while (!WindowShouldClose())
    {
        ball.Update();
        
        BeginDrawing();
            ClearBackground(darkGreen);
            ball.Draw();
        EndDrawing();
    }
    
    CloseWindow();
    */
}