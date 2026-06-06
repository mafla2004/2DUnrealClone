#include <iostream>
#include <raylib.h>
//#include "ball.h"
#include "basegame.h"

#define GAME_LOOP while (!WindowShouldClose())
#define GAME_NAME "2D Unreal with Ruff :3"

#define DRAW(x) BeginDrawing();\
{x}\
EndDrawing();

#define DATASTRUCT_PTRCAST_TEST

constexpr int TargetFramerate = 60;

int main() 
{
#ifdef DATASTRUCT_PTRCAST_TEST
    using namespace std;

    cout << "Starting test on data structures" << endl;
    ArrayList<int> ARR = ArrayList<int>(new int[] {1, 2, 4}, 3);
    LinkedList<int> LL1 = LinkedList<int>(7);
    LinkedList<int> LL2 = LinkedList<int>(12);

    LL1.PushTail(15);
    LL1.PushTail(18);

    LL2.PushTail(24);
    LL2.PushTail(112);

    cout << "ELEMENTS IN ARRAY LIST: " << ARR[0] << ", " << ARR[1] << ", " << ARR[2] << endl;
    cout << "ELEMENTS IN LL 1: " << LL1[0] << ", " << LL1[1] << ", " << LL1[2] << endl;
    cout << "ELEMENTS IN LL 2: " << LL2[0] << ", " << LL2[1] << ", " << LL2[2] << endl;

    LL1.Append(ARR);
    LL1.Append(LL2);
#endif

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