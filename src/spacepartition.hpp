#ifndef __SPACEPART_HPP__
#define __SPACEPART_HPP__

#include "datastructs.h"
#include <raylib.h>

namespace Game
{
    // Fwd declaration
    class Collider;

    //-------------------------------------------------------------------------------------------
    // QUADTREE
    //-------------------------------------------------------------------------------------------

    // A quadtree is a data structure that partitions the space with varying degrees of resolution,
    // Useful to reduce the complexity of collision checking from O(n^2) to O(n log(n))
    class QuadTree
    {
    public: static constexpr uint8 QT_NODE_CAPACITY = 4U;
    private:
        // Size of the region encompassed by this QuadTree.
        Vector2 Size;

        // TODO: Resolve issue if you have one or more colliders centered in the same place, because they
        // could cause the QuadTree to subdivide infinitely.
        Collider **ContainedColliders;

        // Child trees, subdivision of this space, either all null or none null.
        QuadTree *NorthWest;
        QuadTree *NorthEast;
        QuadTree *SouthEast;
        QuadTree *SouthWest;
    public:
        inline QuadTree(const Vector2& _Size) : 
        Size(_Size), NorthWest(nullptr), NorthEast(nullptr), SouthEast(nullptr), SouthWest(nullptr), ContainedColliders(new Collider*[QT_NODE_CAPACITY])
        {

        }

        inline ~QuadTree()
        {
            // Recursively destroy subtrees, then set them to null
            if (NorthWest) delete NorthWest;
            if (NorthEast) delete NorthEast;
            if (SouthEast) delete SouthEast;
            if (SouthWest) delete SouthWest;
            NorthWest = nullptr;
            NorthEast = nullptr;
            SouthEast = nullptr;
            SouthWest = nullptr;
        }
    };
};

#endif