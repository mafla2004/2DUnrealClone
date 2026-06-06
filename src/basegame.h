#ifndef __BASEGAME_H__
#define __BASEGAME_H__

#include "filemanager.h"
#include "datastructs.h"
#include <raylib.h>

namespace Game
{   
    class World
    {

    };

    //-------------------------------------------------------------------------------------------
    // OBJECT 
    //-------------------------------------------------------------------------------------------

    class Object
    {
    private:
    public:
        virtual void Update(Seconds DeltaTime) = 0;
    
        ~Object();
    };

    //-------------------------------------------------------------------------------------------
    // ACTOR 
    //-------------------------------------------------------------------------------------------

    class Actor : public Object
    {
    private:
        Vector2 position;
        Vector2 velocity;

    public:
        inline Vector2 GetPosition();
        inline Vector2 GetVelocity();
        
        virtual void Update(Seconds DeltaTime) override;

        ~Actor();
    };

    inline Vector2 Actor::GetPosition() { return position; }
    inline Vector2 Actor::GetVelocity() { return velocity; }
    
    //-------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------
    // ANIMATION FRAMEWORK
    //-------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------
    
    //-------------------------------------------------------------------------------------------
    // ANIMATION
    //-------------------------------------------------------------------------------------------
    
    // Interface for all objects that represent a 2D animation or a set of 2D animations,
    // this is because an object that uses an animation might want to use a simple animation
    // (a Flipbook), an Animation State Machine, or anything else that handles animations
    class Animation
    {
    protected:
        Seconds AnimTimer;
        
    public:
        virtual Seconds GetDuration() const = 0; 
        virtual Texture GetCurrentFrame() = 0;
    };
    
    //-------------------------------------------------------------------------------------------
    // FLIPBOOK
    //-------------------------------------------------------------------------------------------
    
    // 2D Flipbook animation class that holds a list of sprites (Textures) it will loop through
    // In other words, a simple animation
    class Flipbook : public Animation
    {
    private:
        Texture*    TexArray;
        uint32      Frames;
        uint8       FPS;
    
    public:
        Flipbook& operator=(const Flipbook&);
    
        inline virtual Seconds GetDuration() const final;
    
        Flipbook(Texture*, uint32, uint8);
        Flipbook(const Flipbook&);
        Flipbook(Flipbook&&);
        ~Flipbook() = default;
    };
    
    // May cut-off last frame immediately, if so, change to (double)(Frames + 1) / FPS
    inline Seconds Flipbook::GetDuration() const { return (double)Frames / FPS; }
    
    //-------------------------------------------------------------------------------------------
    // ANIMATION STATE MACHINE
    //-------------------------------------------------------------------------------------------
    
    class AnimationStateMachine : public Animation
    {
        // Can handle animation state machine through a dictionary that holds the state name,
        // the animation object (can be another state machine!) and the nodes it connects to.
    };
};

#endif