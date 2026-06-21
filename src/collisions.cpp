#include <math.h>
#include "collisions.hpp"

// TODO: Add tolerance everywhere because we're working with floats and doubles and machine errors are very real and a bitch.

//-------------------------------------------------------------------------------------------
// COLLIDER
//-------------------------------------------------------------------------------------------

// Maybe I should just pick a collision detection method and stick with it because this is leading to preprocessor abuse...
CollisionState Collider::CheckCollisionAgainst(Collider* OtherCollider, bool CheckChannel)
{
    if (!OtherCollider) return NOT_COLLIDING;

    if (CheckChannel)
    {
        if (GetCollisionResponseToChannel(OtherCollider->GetCollisionChannel()) == IGNORE ||
            OtherCollider->GetCollisionResponseToChannel(GetCollisionChannel()) == IGNORE) return NOT_COLLIDING;
    }

    // Might not be necessary: This makes the function only return overlap if two colliders are only meant to overlap instead of colliding,
    // but here we're only checkin mathematical overlap, not the actual response that should follow.
    //bool ReturnOverlapOnly = CheckChannel ? 
    //    GetCollisionResponseToChannel(OtherCollider->GetCollisionChannel()) == OVERLAP ||
    //    OtherCollider->GetCollisionResponseToChannel(GetCollisionChannel()) == OVERLAP : false;
    double DeltaX = OtherCollider->Position.x - Position.x;
    double DeltaY = OtherCollider->Position.y - Position.y;
#if COLLISION_DETECTION == CDM_AABB
    double MinXDistForCollision = (GetXSize() + OtherCollider->GetXSize()) / 2;
    double MinYDistForCollision = (GetYSize() + OtherCollider->GetYSize()) / 2;
    if (DeltaX > MinXDistForCollision) return NOT_COLLIDING;

    if (DeltaY == MinYDistForCollision) return TOUCHING;
    else if (DeltaY < MinYDistForCollision)
    {
        if (DeltaX == MinXDistForCollision) return TOUCHING;
        return OVERLAPPING;
    }

    return NOT_COLLIDING;
#else 
    double SquaredDistance = DeltaX * DeltaX + DeltaY * DeltaY;
    Vector2 CurrentAxis;

    if (SquaredDistance > GetCheckRadiusSquared() + OtherCollider->GetCheckRadiusSquared()) return NOT_COLLIDING;
#if COLLISION_DETECTION == CDM_SAT_SIMPLE
    // We only have four axes to check against
    // 2 if one collider's axes coincide with the other (that is, if the difference in rotation between the two is some multiple of 90)
    // TODO: Add tolerance due to machine error
    for (uint8 ax = 0; ax < fmod((Rotation - OtherCollider->Rotation), 90.f) ? 4 : 2; ax++)
    {
        switch (ax)
        {
        case 0:
            CurrentAxis = {cosf(Rotation), sinf(Rotation)};
            break;
        case 1:
            CurrentAxis = {-sinf(Rotation), cosf(Rotation)};
            break;
        case 2:
            CurrentAxis = {cosf(OtherCollider->Rotation), sinf(OtherCollider->Rotation)};
            break;
        case 3:
            CurrentAxis = {-sinf(OtherCollider->Rotation), cosf(OtherCollider->Rotation)};
            break;
        default:
            // Something went very, VERY wrong if we're here, ngl.
        }
    }
#elif COLLISION_DETECTION == CDM_SAT_NC
#else
#endif
#endif
}