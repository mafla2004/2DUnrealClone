#ifndef __SPACEPART_HPP__
#define __SPACEPART_HPP__

#include "datastructs.h"
#include "datastructs/ArrayList.hpp"
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
        Vector2 Center;

        // TODO: Resolve issue if you have one or more colliders centered in the same place, because they
        // could cause the QuadTree to subdivide infinitely.
        Collider **ContainedColliders;

        // Child trees, subdivision of this space, either all null or none null.
        QuadTree *NorthWest;
        QuadTree *NorthEast;
        QuadTree *SouthEast;
        QuadTree *SouthWest;

        inline bool Subdivide();
    public:
        inline bool IsSubdivided();
        inline bool Contains(Collider*);
        inline bool Insert(Collider*);
        inline bool Remove(Collider*);

        // Returns a collection of colliders the current collider may be colliding with.
        ArrayList<Collider*> Query(Collider*);

        inline QuadTree(const Vector2& _Size) : 
        Size(_Size), NorthWest(nullptr), NorthEast(nullptr), SouthEast(nullptr), SouthWest(nullptr), ContainedColliders(new Collider*[QT_NODE_CAPACITY])
        {

        }
        inline QuadTree(Vector2&& _Size) : 
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

    inline bool QuadTree::IsSubdivided() { return NorthWest != nullptr; }

    inline bool QuadTree::Contains(Collider* col)
    {
        for (uint8 i = 0; i < QT_NODE_CAPACITY; i++)
        {
            if (ContainedColliders[i] == col) return true;
        }

        if (NorthWest->Contains(col)) return true;
        if (NorthEast->Contains(col)) return true;
        if (SouthWest->Contains(col)) return true;
        if (SouthEast->Contains(col)) return true;

        return false;
    }

    inline bool QuadTree::Subdivide()
    {
        // Already subdivided
        if (IsSubdivided()) return false;
    }
};

#endif