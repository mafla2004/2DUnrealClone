#include <iostream>
#include <raylib.h>
//#include "ball.h"
#include "basegame.h"

#define GAME_LOOP while (!WindowShouldClose())
#define GAME_NAME "2D Unreal with Ruff :3"

#define DRAW(x) BeginDrawing();\
{x}\
EndDrawing();

#define LLIST 0
#define DICT 1

#define TEST_ON LLIST

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

#if TEST_ON == DICT
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

    TestDict["Mattia"] = 45;
    TestDict["Horus"] = 80;
    TestDict["Ruff"] = 69;

    std::cout << "ITEMS CONTAINED ARE NOW " << TestDict.GetCount() << std::endl;

    for (Pair<String, uint16>& p : TestDict)
    {
        std::cout << "PRINTING ELEMENT" << std::endl;
        std::cout << "CURRENT ITEM: " << p.GetKey() << " " << p.GetValue() << std::endl;
    }
#elif TEST_ON == LLIST
    std::cout << "STARTING TEST ON LINKED LISTS" << std::endl;

    LinkedList<uint16> TestList = LinkedList<uint16>(0);

    TestList.PushTail(1);
    TestList.PushTail(2);
    TestList.PushTail(4);
    TestList.PushTail(28);
    TestList.PushTail(42);
    TestList.PushTail(71);

    std::cout << "List size is: " << TestList.GetSize() << std::endl;
    std::cout << "Iterating on list!" << std::endl;

    for (uint16 i : TestList)
    {
        std::cout << i << " ";
    }

    std::cout << std::endl;

    std::cout << "TESTING REMOVAL, REMOVING ELEMENT 28" << std::endl;
    for (auto it = TestList.begin(), end = TestList.end(); it != end;)
    {
        std::cout << "ITEM: " << *it << std::endl;

        if (*it == 28)
        {
            std::cout << "REMOVING" << std::endl;
            it = TestList.Erase(it);
            continue;
        }

        ++it;
    }

    std::cout << "LIST NOW CONTAINS: ";
    for (uint16 i : TestList)
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "INSERTING IN FRONT OF 4" << std::endl;
    for (auto it = TestList.begin(), end = TestList.end(); it != end;)
    {
        if (*it == 4)
        {
            std::cout << "ADDING" << std::endl;
            TestList.Add(it, 69);
        }

        ++it;
    }

    std::cout << "LIST NOW CONTAINS: ";
    for (uint16 i : TestList)
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "TESTING REMOVALOF FIRST ELEMENT" << std::endl;
    for (auto it = TestList.begin(), end = TestList.end(); it != end;)
    {
        if (*it == 0)
        {
            std::cout << "REMOVING" << std::endl;
            it = TestList.Erase(it);
            continue;
        }

        ++it;
    }

    std::cout << "LIST NOW CONTAINS: ";
    for (uint16 i : TestList)
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;
#endif
    
    GAME_LOOP
    {
        DRAW(
            ClearBackground(BLACK);
            DrawText("Unreal with Ruff :3", 190, 200, 20, LIGHTGRAY);
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