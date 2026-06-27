#include "spacepart.hpp"
#include "collisions.hpp"
#include <raylib.h>
#include <math.h>

using namespace Game;

inline bool QuadTree::Fits(Collider* col)
{ if (!col) return false;
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

inline bool QuadTree::Overlaps(Collider* col)
{ if (!col) return false;
    Vector2 RelPos = col->GetPosition() - Center;
    double SqrRad = col->GetCheckRadiusSquared(); 
#if INSERTION_REGIMENT == IR_CENTERBASED
    return abs(RelPos.x) <= Size.x && abs(RelPos.y) <= Size.y;
#elif INSERTION_REGIMENT == IR_RADIUSBASED
    float XDistSqr = RelPos.x * RelPos.x;
    float YDistSqr = RelPos.y * RelPos.y;
    return XDistSqr <= (Size.x * Size.x + SqrRad) && YDistSqr <= (Size.y * Size.y + SqrRad);
#else
    // TODO: Implement
#endif
}

inline bool QuadTree::Insert(Collider* col)
{
    if (!Overlaps(col) || Contains(col)) return false;

    // Insertion regime goes as follows:
    // Try to insert the colliders in the lowest possible level of the tree,
    // to do this we add a collider to the current level if the tree isn't subdivided,
    // then we see if we need to subdivide or not, and in case we need to subdivide
    // we move all colliders down; if the tree was already subdivided, we move this collider down.

    if (!IsSubdivided())
    {
        ContainedColliders.PushHead(col);

        if (ContainedColliders.GetSize() <= QT_NODE_CAPACITY) return true;

        // If no collider (including the one that we're trying to insert) would completely fit in the sub quad trees, then it's useless to subdivide.
        float HalfXSize = Size.x / 2;
        float HalfYSize = Size.y / 2;

        bool ShouldSubdivide = false;
        for (Collider* c : ContainedColliders)
        {
            // Check to see if there is at least one collider which is smaller than the hypothetical subtrees,
            // otherwise subdivision is useless and we'd end up with 4 subtrees that are equally as crowded.
            if (c->GetCheckRadiusSquared() > fmin(HalfXSize * HalfXSize, HalfYSize * HalfYSize)) continue;

            ShouldSubdivide = true;
            break;
        }

        if (ShouldSubdivide)
        {
            Subdivide();

            // Move the colliders down
            for (Collider* c : ContainedColliders)
            {
                if (NorthWest->Overlaps(c)) NorthWest->Insert(c);
                if (NorthEast->Overlaps(c)) NorthEast->Insert(c);
                if (SouthEast->Overlaps(c)) SouthEast->Insert(c);
                if (SouthWest->Overlaps(c)) SouthWest->Insert(c);
            }

            ContainedColliders.Clear();
        }

        return true;
    }

    // Tree is subdivided, move this collider down.
    if (NorthWest->Overlaps(col)) NorthWest->Insert(col);
    if (NorthEast->Overlaps(col)) NorthEast->Insert(col);
    if (SouthEast->Overlaps(col)) SouthEast->Insert(col);
    if (SouthWest->Overlaps(col)) SouthWest->Insert(col);

    return true;
}