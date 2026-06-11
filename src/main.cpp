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

    std::cout << "STARTING TEST ON DICTIONARIES" << std::endl;

    Dictionary<String, uint16> TestDict = Dictionary<String, uint16>([=](const String& str) -> uint16 {
        uint16 Hash = 0;
        uint16 Prime = 1;
        for (const char& c : str)
        {
            Hash += c * Prime;
            Prime *= 53;
        }
        return Hash;
    }, 512U);

    std::cout << "CREATED DICTIONARY OF SIZE " << TestDict.GetSize() << " WITH " << TestDict.GetCount() << " ITEMS INSIDE" << std::endl;
    std::cout << "TRYING INSERTION VIA SUBSCRIPT OPERATOR: " << std::endl;

    TestDict["Vittroia"] = 17;

    std::cout << "ITEMS CONTAINED ARE NOW " << TestDict.GetCount() << std::endl;

    if (TestDict.GetCount())
    {
        std::cout << "ELEMENT ASSOCIATED WITH Vittroia IS: " << TestDict["Vittroia"] << std::endl;
        std::cout << "ITEMS CONTAINED ARE NOW " << TestDict.GetCount() << std::endl;
    }
    
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