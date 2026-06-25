#include <math.h>
#include "math/vectormath.hpp"
#include "collisions.hpp"

// TODO: Add tolerance everywhere because we're working with floats and doubles and machine errors are very real and a bitch.

//-------------------------------------------------------------------------------------------
// COLLIDER
//-------------------------------------------------------------------------------------------

using namespace Game;

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
    float DeltaX = OtherCollider->Position.x - Position.x;
    float DeltaY = OtherCollider->Position.y - Position.y;
#if COLLISION_DETECTION == CDM_AABB
    float MinXDistForCollision = (GetXSize() + OtherCollider->GetXSize()) / 2;
    float MinYDistForCollision = (GetYSize() + OtherCollider->GetYSize()) / 2;
    if (DeltaX > MinXDistForCollision) return NOT_COLLIDING;

    if (DeltaY == MinYDistForCollision) return TOUCHING;
    else if (DeltaY < MinYDistForCollision)
    {
        if (DeltaX == MinXDistForCollision) return TOUCHING;
        return OVERLAPPING;
    }

    return NOT_COLLIDING;
#else 
    float SquaredDistance = DeltaX * DeltaX + DeltaY * DeltaY;
    Vector2 CurrentAxis;

    if (SquaredDistance > GetCheckRadiusSquared() + OtherCollider->GetCheckRadiusSquared()) return NOT_COLLIDING;
#if COLLISION_DETECTION == CDM_SAT_SIMPLE
    // We only have four axes to check against
    // 2 if one collider's axes coincide with the other (that is, if the difference in rotation between the two is some multiple of 90)
    // TODO: Add tolerance due to machine error
    float CosThis = cosf(Rotation);
    float SinThis = sinf(Rotation);
    float CosOther = cosf(OtherCollider->Rotation);
    float SinOther = sinf(OtherCollider->Rotation);
    for (uint8 ax = 0; ax < (fmod((Rotation - OtherCollider->Rotation), 90.f) ? 4 : 2); ax++)
    {
        switch (ax)
        {
        case 0:
            CurrentAxis = {CosThis, SinThis};
            break;
        case 1:
            CurrentAxis = {-SinThis, CosThis};
            break;
        case 2:
            CurrentAxis = {CosOther, SinOther};
            break;
        case 3:
            CurrentAxis = {-SinOther, CosOther};
            break;
        }

        float Min1, Max1, Min2, Max2;
        float proj;

        // Calculate projections

        // This collider
        // TODO: There's probably a way to do this a lot quicker, without iterating through all 4 points, but I can't for the life of me figure it out
        // right now because I'm tired and angry and I don't know the reason for it.
        // TODO: Possible optimization: just calculate the X and Y extension based on rectangle size duh, FIX!!!
        for (uint8 p = 0; p <= 4; p++)
        {
            Vector2 point;
            switch (p)  // Unrotated point
            {
            case 0:
                point = {Position.x - GetXSize() / 2, Position.y - GetYSize() / 2};
                break;
            case 1:
                point = {Position.x - GetXSize() / 2, Position.y + GetYSize() / 2};
                break;
            case 2:
                point = {Position.x + GetXSize() / 2, Position.y - GetYSize() / 2};
                break;
            case 3:
                point = {Position.x + GetXSize() / 2, Position.y + GetYSize() / 2};
                break;
            }

            // Rotate the point
            point = {point.x * CosThis + point.y * SinThis, point.y * CosThis - point.x * SinThis};
            // Project
            proj = CurrentAxis * point;

            if (!p)
            {
                Min1 = Max1 = proj;
                continue;
            }

            if (proj < Min1)
            {
                Min1 = proj;
                continue;
            }

            if (proj > Max1) Max1 = proj;
        }

        // Other collider
        for (uint8 p = 0; p <= 4; p++)
        {
            Vector2 point;
            switch (p)  // Unrotated point
            {
            case 0:
                point = {OtherCollider->Position.x - OtherCollider->GetXSize() / 2, OtherCollider->Position.y - OtherCollider->GetYSize() / 2};
                break;
            case 1:
                point = {OtherCollider->Position.x - OtherCollider->GetXSize() / 2, OtherCollider->Position.y + OtherCollider->GetYSize() / 2};
                break;
            case 2:
                point = {OtherCollider->Position.x + OtherCollider->GetXSize() / 2, OtherCollider->Position.y - OtherCollider->GetYSize() / 2};
                break;
            case 3:
                point = {OtherCollider->Position.x + OtherCollider->GetXSize() / 2, OtherCollider->Position.y + OtherCollider->GetYSize() / 2};
                break;
            }

            // Rotate the point
            point = {point.x * CosOther + point.y * SinOther, point.y * CosOther - point.x * SinOther};
            // Project
            proj = CurrentAxis * point;

            if (!p)
            {
                Min2 = Max2 = proj;
                continue;
            }

            if (proj < Min2)
            {
                Min2 = proj;
                continue;
            }

            if (proj > Max2) Max2 = proj;
        }

        // Compare projection
        if (Min2 <= Max1 || Min1 <= Max2)
        {
            if (Min2 == Max1 || Min1 == Max2) return TOUCHING;

            return OVERLAPPING;
        }
    }

    return NOT_COLLIDING;
#elif COLLISION_DETECTION == CDM_SAT_NC
#else
#endif
#endif
}

//-------------------------------------------------------------------------------------------
// RECTANGLE
//-------------------------------------------------------------------------------------------

inline float Rect::GetXSize() { return size.x; }
inline float Rect::GetYSize() { return size.y; }

// The way this checks for overlap of a point with a ractangle is by projecting the point onto the two
// relative axes, then considers the distance of the point on each axis from the center of the rectangle
// and checks that the distance is less than the size.
bool Rect::ContainsPoint(const Vector2& point)
{
    Vector2 RelativeX, RelativeY;
    float XProj, YProj, CXProj, CYProj;

    RelativeX = {cosf(Rotation), sinf(Rotation)};
    RelativeY = {-sinf(Rotation), cosf(Rotation)};

    CXProj = Position * RelativeX;
    CYProj = Position * RelativeY;

    XProj = point * RelativeX - CXProj;
    YProj = point * RelativeY - CYProj;

    return fabs(XProj) <= size.x && fabs(YProj) <= size.y;
}

double Rect::GetCheckRadiusSquared()
{
    return size * size;
}