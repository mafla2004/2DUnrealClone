#ifndef __COLL_HPP__
#define __COLL_HPP__

#include "datastructs.h"
#include "datastructs\LinkedList.hpp"
#include <raylib.h>

#define CDM_AABB 0  // Axis Aligned Bounding Boxes

#define COLLISION_DETECTION CDM_AABB

// Fwd Declarations
class Collider;

//-------------------------------------------------------------------------------------------
// COLLISION INFORMATION ENUMS
//-------------------------------------------------------------------------------------------

// Defines the collision type for a specific channel.
// Two objects can collide if and only if both are set to "block" for their specific channel.
// E.G. If an object of channel WORLD_DYNAMIC starts to intersect with another object of channel STATIC,
// the collision will only happen if the first object has assigned BLOCK to STATIC and the second has
// assigned BLOCK to WORLD_DYNAMIC, otherwise they will phase through each other and, unless the collisions are
// set to IGNORE, they will trigger a BeginOverlap event.
enum CollisionType
{
    BLOCK,
    OVERLAP,
    IGNORE
};

// Defines the different channels of collision.
// STATIC - Immovable objects, assumed to be part of the world.
// WORLD_DYNAMIC - Objects that are part of the world and can move.
// PAWN - Entities like the player and monsters.
// These, in combination with CollisionType, are useful for optimizing collision checks:
// Only WORLD_DYNAMIC and PAWN colliders have to check collisions against the other objects in the world,
// if one channel is set to ignore, then the check against objects of that same channel are skipped altogether.
// Objects of each collision channel will be stored in their own LinkedList, this makes skipping a channel instant.
enum CollisionChannel
{
    STATIC,
    WORLD_DYNAMIC,
    PAWN
};
constexpr uint8 CollisionChannels = 3U;

//-------------------------------------------------------------------------------------------
// COLLISION PROFILE
//-------------------------------------------------------------------------------------------

// Contains all the info about the collisions of a certain collider.
struct CollisionProfile
{
    CollisionChannel        Channel;                    
    CollisionType*          CollisionBehaviours;
    LinkedList<Collider*>   IgnoredColliders;

    inline explicit CollisionProfile(CollisionChannel, CollisionType*, uint8);
    inline explicit CollisionProfile(CollisionChannel);
};

inline CollisionProfile::CollisionProfile(CollisionChannel channel)
{
    Channel = channel;
    CollisionBehaviours = new CollisionType[CollisionChannels];
    CollisionBehaviours[STATIC]         = BLOCK;
    CollisionBehaviours[WORLD_DYNAMIC]  = BLOCK;
    CollisionBehaviours[PAWN]           = BLOCK;
    IgnoredColliders = LinkedList<Collider*>();
}

inline CollisionProfile::CollisionProfile(CollisionChannel channel, CollisionType* collisionBehaviours, uint8 size)
{
    Channel = channel;
    CollisionBehaviours = new CollisionType[CollisionChannels];
    CollisionBehaviours[STATIC]         = BLOCK;
    CollisionBehaviours[WORLD_DYNAMIC]  = BLOCK;
    CollisionBehaviours[PAWN]           = BLOCK;
    IgnoredColliders = LinkedList<Collider*>();

    if (!collisionBehaviours || !size)
    {
        return;
    }

    for (uint8 i = 0; i < (size < CollisionChannels ? size : CollisionChannels); i++)
    {
        CollisionBehaviours[i] = collisionBehaviours[i];
    }
}

//-------------------------------------------------------------------------------------------
// COLLIDER
//-------------------------------------------------------------------------------------------

// Generic collider base class, abstract
class Collider
{
protected:
    Vector2 position;


public:
    //void OnBeginOverlap();
};

//-------------------------------------------------------------------------------------------
// RECTANGLE
//-------------------------------------------------------------------------------------------

class Rectangle : public Collider
{
private:
    Vector2 size;

public:

};

#endif