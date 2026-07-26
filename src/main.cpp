#include <iostream>
#include <raylib.h>
//#include "ball.h"
#include "math.h"
#include "basegame.h"
#include "collisions.hpp"
#include "math/vectormath.hpp"

#define GAME_LOOP while (!WindowShouldClose())
#define GAME_NAME "2D Unreal with Ruff :3"

#define DRAW(x) BeginDrawing();\
{x}\
EndDrawing();

#define LLIST 0
#define DICT 1

#define TEST_ON 90

constexpr int TargetFramerate = 60;

int main() 
{
    using namespace Game;

    Resolution res;
    res = {1024, 720};

    InitWindow(res.x, res.y, GAME_NAME);
    SetTargetFPS(TargetFramerate);

    constexpr float MOV_FACTOR = 1.f;

    CollisionType Responses[] = {OVERLAP, OVERLAP, OVERLAP, OVERLAP};
    CollisionProfile RectProfile(WORLD_DYNAMIC, Responses, 4);

    Rect *R1, *R2;
    R1 = new Rect({120, 300}, {100, 100}, PI/4, RectProfile);
    R2 = new Rect({120 + sqrtf(R1->GetCheckRadiusSquared()) + 25 , 350}, {50, 100}, 0, RectProfile);

    CollisionState ColState = NOT_COLLIDING;

    GAME_LOOP
    {
        DRAW(
            ClearBackground(BLACK);

            DrawText("Use WASD to move the left rectangle and the Arrow Keys to move the right rectangle. Woof :3", 0, 0, 15, GREEN);

            if (IsKeyDown(KEY_W))       R1->SetPosition(R1->GetPosition() - (Vector2){0, MOV_FACTOR});
            if (IsKeyDown(KEY_S))       R1->SetPosition(R1->GetPosition() + (Vector2){0, MOV_FACTOR});
            if (IsKeyDown(KEY_A))       R1->SetPosition(R1->GetPosition() - (Vector2){MOV_FACTOR, 0});
            if (IsKeyDown(KEY_D))       R1->SetPosition(R1->GetPosition() + (Vector2){MOV_FACTOR, 0});

            if (IsKeyDown(KEY_UP))      R2->SetPosition(R2->GetPosition() - (Vector2){0, MOV_FACTOR});
            if (IsKeyDown(KEY_DOWN))    R2->SetPosition(R2->GetPosition() + (Vector2){0, MOV_FACTOR});
            if (IsKeyDown(KEY_LEFT))    R2->SetPosition(R2->GetPosition() - (Vector2){MOV_FACTOR, 0});
            if (IsKeyDown(KEY_RIGHT))   R2->SetPosition(R2->GetPosition() + (Vector2){MOV_FACTOR, 0});

            ColState = R2->CheckCollisionAgainst(R1, false);

            switch (ColState)
            {
            case NOT_COLLIDING: DrawText("Not Colliding", 412, 540, 20, RED);   break;
            case TOUCHING:      DrawText("Touching UwU", 412, 540, 20, YELLOW); break;
            case OVERLAPPING:   DrawText("Overlapping", 412, 540, 20, GREEN);   break;
            }

            R1->DebugDraw();
            R2->DebugDraw();
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