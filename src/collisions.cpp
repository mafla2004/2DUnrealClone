#include "collisions.hpp"

//-------------------------------------------------------------------------------------------
// COLLIDER
//-------------------------------------------------------------------------------------------

CollisionState Collider::CheckCollisionAgainst(Collider* OtherCollider, bool CheckChannel)
{
    if (!OtherCollider) return NOT_COLLIDING;

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

    if (SquaredDistance > GetCheckRadiusSquared() + OtherCollider->GetCheckRadiusSquared()) return NOT_COLLIDING;
#endif
}