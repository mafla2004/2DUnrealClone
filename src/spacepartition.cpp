#include "spacepartition.hpp"
#include "collisions.hpp"
#include <raylib.h>
#include <math.h>

using namespace Game;

inline bool QuadTree::Fits(Collider* col)
{
    Vector2 RelPos = col->GetPosition() - Center;
    double SqrRad = col->GetCheckRadiusSquared();
#if INSERTION_REGIMENT == IR_CENTERBASED
    return abs(RelPos.x) <= Size.x && abs(RelPos.y) <= Size.y;    // Anything fits bruh.
#elif INSERTION_REGIMENT == IR_RADIUSBASED
    // TODO: Find a way to avoid the square root.
    double Radius = sqrt(SqrRad);
    return (RelPos.x + Radius) <= Size.x && (RelPos.x - Radius) >= -Size.x && (RelPos.y + Radius) <= Size.y && (RelPos.y - Radius) >= Size.y;
#else // Exact inserion
    // TODO: Implement
#endif
}