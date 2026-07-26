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
    float Distance = sqrtf(DeltaX * DeltaX + DeltaY * DeltaY);

    if (Distance > sqrtf(GetCheckRadiusSquared()) + sqrtf(OtherCollider->GetCheckRadiusSquared())) return NOT_COLLIDING;
#if COLLISION_DETECTION == CDM_SAT_SIMPLE
    // We only have four axes to check against
    // 2 if one collider's axes coincide with the other (that is, if the difference in rotation between the two is some multiple of 90)
    // TODO: Add tolerance due to machine error
    float CosThis   = cosf(Rotation);
    float SinThis   = sinf(Rotation);
    float CosOther  = cosf(OtherCollider->Rotation);
    float SinOther  = sinf(OtherCollider->Rotation);
    
    float HalfXThis  = GetXSize() / 2;
    float HalfYThis  = GetYSize() / 2;
    float HalfXOther = OtherCollider->GetXSize() / 2;
    float HalfYOther = OtherCollider->GetYSize() / 2;

    bool bTouch = false;    // This boolean flag is set to true in the case in which the collider touch without overlapping on one axis,
                            // The algorithm first determines whether or not there is a separating axis between the colliders, and if there is none
                            // it detects if the colliders touch without overlapping on one normal, if they do they're touching, otherwise they're
                            // overlapping

    // 0 and 1 are normals of this collider, 2 and 3 of the other collider
    Vector2 Normals[] = {
        {CosThis, -SinThis},
        {SinThis, CosThis},
        {CosOther, -SinOther},
        {SinOther, CosOther}
    };

    Vector2 ThesePoints[4];
    Vector2 OtherPoints[4];

    for (uint8 p = 0; p < 4; p++)
    {
        switch (p)
        {
        case 0: 
            ThesePoints[p] = {-HalfXThis, -HalfYThis};
            OtherPoints[p] = {-HalfXOther, -HalfYOther};
            break;
        case 1: 
            ThesePoints[p] = {-HalfXThis, HalfYThis};
            OtherPoints[p] = {-HalfXOther, HalfYOther};
            break;
        case 2:
            ThesePoints[p] = {HalfXThis, HalfYThis};
            OtherPoints[p] = {HalfXOther, HalfYOther};
            break;
        case 3:
            ThesePoints[p] = {HalfXThis, -HalfYThis};
            OtherPoints[p] = {HalfXOther, -HalfYOther};
            break;
        }

        // Rotate and traslate point
        ThesePoints[p] = (Vector2){ThesePoints[p].x * CosThis + ThesePoints[p].y * SinThis, ThesePoints[p].y * CosThis - ThesePoints[p].x * SinThis} + Position;
        OtherPoints[p] = (Vector2){OtherPoints[p].x * CosOther + OtherPoints[p].y * SinOther, OtherPoints[p].y * CosOther - OtherPoints[p].x * SinOther} + OtherCollider->Position;
    }

    // Iterate over normals
    for (uint8 n = 0; n < 4; n++)
    {
        float MinThis, MaxThis;
        float MinOther, MaxOther;

        // Iterate over the points of both colliders & project them on the normal
        for (uint8 p = 0; p < 4; p++)
        {
            float ProjThis  = ThesePoints[p] * Normals[n];
            float ProjOther = OtherPoints[p] * Normals[n];

            if (!p)
            {
                MinThis = MaxThis = ProjThis;
                MinOther = MaxOther = ProjOther;
                continue;
            }

            if (ProjThis < MinThis)         MinThis = ProjThis; 
            else if (ProjThis > MaxThis)    MaxThis = ProjThis;

            if (ProjOther < MinOther)       MinOther = ProjOther;
            else if (ProjOther > MaxOther)  MaxOther = ProjOther;
        }

        // DEBUG - Draw normals and projections
        Vector2 Center = (n < 2 ? Position : OtherCollider->Position);
        float CenterProj = Center * Normals[n];

        Vector2 DBStartPos1 = Center - Normals[n] * 750.f;
        Vector2 DBEndPos1   = Center + Normals[n] * 750.f;
        Vector2 DBProjPt1   = Center + Normals[n] * (MinThis - CenterProj);
        Vector2 DBProjPt2   = Center + Normals[n] * (MaxThis - CenterProj);
        Vector2 DBProjPt3   = Center + Normals[n] * (MinOther - CenterProj);
        Vector2 DBProjPt4   = Center + Normals[n] * (MaxOther - CenterProj);
        
        DrawLine(DBStartPos1.x, DBStartPos1.y, DBEndPos1.x, DBEndPos1.y, YELLOW);
        DrawCircle(DBProjPt1.x, DBProjPt1.y, 5.f, RED);
        DrawCircle(DBProjPt2.x, DBProjPt2.y, 5.f, RED);
        DrawCircle(DBProjPt3.x, DBProjPt3.y, 5.f, BLUE);
        DrawCircle(DBProjPt4.x, DBProjPt4.y, 5.f, BLUE);

        if (MinThis > MaxOther)     return NOT_COLLIDING;
        if (MinOther > MaxThis)     return NOT_COLLIDING;

        if (bTouch) continue; 
        if ((MinThis == MaxOther) || (MinOther == MaxThis)) bTouch = true;
    }

    return bTouch ? TOUCHING : OVERLAPPING;
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