#ifndef __COLL_HPP__
#define __COLL_HPP__

#include "datastructs.h"
#include "datastructs\LinkedList.hpp"
#include "basegame.h"
#include <raylib.h>

#define CDM_AABB        0  // Axis Aligned Bounding Boxes - Easiest collision detection method but very limited, requires all colliders to be rectangles that are perfectly aligned with the axes. Does not support slopes, triangles, n-gons or circles.
#define CDM_SAT_SIMPLE  1  // Collision Detection based on the Separating Axis Theorem, simplified - Another extremely easy collision detection method that allows colliders to not be aligned with the axes, but still forces them to be rectangles or cicles. Supports slopes and circles, doesn't support triangles and n-gons.
#define CDM_SAT_NC      2  // Collision Detection based on the Separating Axis Theorem, with added support for n-gons that are not convex.
#define CDM_SAT_FULL    3  // Collision Detection based on the Separating Axis Theorem, full support for any n-gon added.

#define COLLISION_DETECTION CDM_SAT_SIMPLE

namespace Game
{
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
        ENTITY,
        PAWN
    };
    constexpr uint8 CollisionChannels = 4U;

    // Enum for the collision state between two colliders.
    // NOT_COLLIDING means the two colliders are not colliding,
    // TOUCHING means the two colliders are touching but not overlapping,
    // OVERLAPPING means that the two colliders are overlapping, adjustment has to be made (if possible) to make the two objects touch instead of overlapping.
    enum CollisionState
    {
        NOT_COLLIDING,
        TOUCHING,
        OVERLAPPING
    };

    //-------------------------------------------------------------------------------------------
    // COLLISION PROFILE
    //-------------------------------------------------------------------------------------------

    // Contains all the info about the collisions of a certain collider.
    struct CollisionProfile
    {
        CollisionChannel        Channel;                    
        CollisionType*          CollisionBehaviours;
        LinkedList<Collider*>   IgnoredColliders;

        bool AddIgnoredCollider(Collider*);
        bool RemoveIgnoredCollider(Collider*);

        inline explicit CollisionProfile(CollisionChannel, CollisionType*, uint8);
        inline explicit CollisionProfile(CollisionChannel);
    };

    inline CollisionProfile::CollisionProfile(CollisionChannel channel)
    {
        Channel = channel;
        CollisionBehaviours = new CollisionType[CollisionChannels];
        CollisionBehaviours[STATIC]         = BLOCK;
        CollisionBehaviours[WORLD_DYNAMIC]  = BLOCK;
        CollisionBehaviours[ENTITY]         = BLOCK;
        CollisionBehaviours[PAWN]           = BLOCK;
        IgnoredColliders = LinkedList<Collider*>();
    }

    inline CollisionProfile::CollisionProfile(CollisionChannel channel, CollisionType* collisionBehaviours, uint8 size)
    {
        // TODO: Remake in a better way, you can remove the manual initializations.
        Channel = channel;
        CollisionBehaviours = new CollisionType[CollisionChannels];
        CollisionBehaviours[STATIC]         = BLOCK;
        CollisionBehaviours[WORLD_DYNAMIC]  = BLOCK;
        CollisionBehaviours[ENTITY]         = BLOCK;
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
    class Collider //: public Actor
    {
    protected:
        Vector2             Position;
        CollisionProfile    ColProfile;
#if COLLISION_DETECTION != CDM_AABB
        float               Rotation;
#endif

        // TODO: Consider making public
        inline Collider(const Vector2& _pos, float _rot, CollisionProfile _profile = CollisionProfile(WORLD_DYNAMIC)) : 
        Position(_pos), Rotation(_rot), ColProfile(_profile) {}

    public:
        //void OnBeginOverlap();

        // NOTE: These probably shouldn't be here.
        virtual float GetXSize() = 0;
        virtual float GetYSize() = 0;
        virtual Vector2 GetSize() = 0;

        // Returns the squared radius of the circle that inscribes the collider (centered in the collider's position), 
        // which is the (squared) distance beyond which we know for sure a surface/point cannot possibly be colliding with the collider.
        // TODO: Maybe change to float.
        virtual double GetCheckRadiusSquared() = 0;
        virtual bool ContainsPoint(const Vector2&) = 0;

        inline CollisionChannel GetCollisionChannel();
        inline CollisionProfile GetCollisionProfile();
        inline CollisionType GetCollisionResponseToChannel(CollisionChannel);
        inline Vector2 GetPosition();
#if COLLISION_DETECTION != CDM_AABB
        inline float GetRotation();
        inline void SetRotation(float);
        inline void RotateBy(float);
#endif
        inline void SetPosition(const Vector2&);

        virtual void DebugDraw() = 0;

        CollisionState CheckCollisionAgainst(Collider*, bool = false);
    };

    inline CollisionChannel Collider::GetCollisionChannel() { return ColProfile.Channel; }
    inline CollisionProfile Collider::GetCollisionProfile() { return ColProfile; }
    inline CollisionType Collider::GetCollisionResponseToChannel(CollisionChannel Channel) { return ColProfile.CollisionBehaviours[Channel]; }
    inline Vector2 Collider::GetPosition() { return Position; }
#if COLLISION_DETECTION != CDM_AABB
    inline float Collider::GetRotation() { return Rotation; }
    inline void Collider::SetRotation(float newRot) { Rotation = newRot; }
    inline void Collider::RotateBy(float rot) { Rotation += rot; }
#endif
    inline void Collider::SetPosition(const Vector2& newPos) { Position = newPos; }

    //-------------------------------------------------------------------------------------------
    // RECTANGLE
    //-------------------------------------------------------------------------------------------

    class Rect : public Collider
    {
    private:
       Vector2 size;

    public:
        // TODO: Remove or move to Collider...
        inline void SetSize(const Vector2&);

        virtual float GetXSize()    override final;
        virtual float GetYSize()    override final;
        virtual Vector2 GetSize()   override final;

        virtual double GetCheckRadiusSquared()      override;
        virtual bool ContainsPoint(const Vector2&)  override;

        virtual void DebugDraw()                    override;

        inline Rect(const Vector2& _pos, const Vector2& _size, float _rot = 0.f, CollisionProfile _profile = CollisionProfile(WORLD_DYNAMIC)) : size(_size), Collider(_pos, _rot, _profile) {}
        ~Rect();
    };

    inline void Rect::SetSize(const Vector2& newSize) { size = newSize; }

    // TODO: Maybe add circles since they're easy to fake in all collision detection algorithms.
};

#endif