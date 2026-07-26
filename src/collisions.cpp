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
                point = {-GetXSize() / 2, -GetYSize() / 2};
                break;
            case 1:
                point = {-GetXSize() / 2, GetYSize() / 2};
                break;
            case 2:
                point = {GetXSize() / 2, -GetYSize() / 2};
                break;
            case 3:
                point = {GetXSize() / 2, GetYSize() / 2};
                break;
            }

            // Rotate the point
            point = {point.x * CosThis + point.y * SinThis, point.y * CosThis - point.x * SinThis};
            point = point + Position;
            // Project
            proj = CurrentAxis * point;

            // Change from previous commit: Possible mathematical mistake - we added the center of the collider before rotating, 
            // which doesn't rotate the point relative to the center but rather rotates the entire shape respective to 0,0. Corrected this.

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
                point = {-OtherCollider->GetXSize() / 2, -OtherCollider->GetYSize() / 2};
                break;
            case 1:
                point = {-OtherCollider->GetXSize() / 2, OtherCollider->GetYSize() / 2};
                break;
            case 2:
                point = {OtherCollider->GetXSize() / 2, -OtherCollider->GetYSize() / 2};
                break;
            case 3:
                point = {OtherCollider->GetXSize() / 2, OtherCollider->GetYSize() / 2};
                break;
            }

            // Rotate the point
            point = {point.x * CosOther + point.y * SinOther, point.y * CosOther - point.x * SinOther};
            point = point + OtherCollider->Position;
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
        if (Min2 >= Max1 || Min1 >= Max2)
        {
            return NOT_COLLIDING;
        }

        if (Min2 == Max1 || Min1 == Max2) return TOUCHING;
        return OVERLAPPING;
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
    return (size/2) * (size/2);
}

void Rect::DebugDraw()
{
    Vector2 Points[4];
    float Sin;
    float Cos;

    Sin = sinf(GetRotation());
    Cos = cosf(GetRotation());

    for (uint8 i = 0U; i < 4U; i++)
    {
        Points[i] = {0};

        if (i < 2)          Points[i].x -= GetXSize() / 2;
        else                Points[i].x += GetXSize() / 2;

        if (i > 0 && i < 3) Points[i].y += GetYSize() / 2;
        else                Points[i].y -= GetYSize() / 2;

        // Rotate point and add the center
        Points[i] = {Points[i].x * Cos + Points[i].y * Sin, Points[i].y * Cos - Points[i].x * Sin};
        Points[i] = Points[i] + GetPosition();

        DrawCircle(Points[i].x, Points[i].y, 2, RED);
    }

    DrawLine(Points[0].x, Points[0].y, Points[1].x, Points[1].y, YELLOW);
    DrawLine(Points[1].x, Points[1].y, Points[2].x, Points[2].y, YELLOW);
    DrawLine(Points[2].x, Points[2].y, Points[3].x, Points[3].y, YELLOW);
    DrawLine(Points[3].x, Points[3].y, Points[0].x, Points[0].y, YELLOW);

    DrawCircle(Position.x, Position.y, sqrtf(GetCheckRadiusSquared()), {0, 255, 0, 64});
}

Vector2 Rect::GetSize() { return size; }