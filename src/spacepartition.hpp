#ifndef __SPACEPART_HPP__
#define __SPACEPART_HPP__

#include "datastructs.h"
#include "datastructs/LinkedList.hpp"
#include "datastructs/ArrayList.hpp"
#include "math/vectormath.hpp"
#include <raylib.h>

#define IR_CENTERBASED 0
#define IR_RADIUSBASED 1
#define IR_EXACT 2

#define INSERTION_REGIMENT IR_RADIUSBASED

namespace Game
{
    // Fwd declaration
    class Collider;

    //-------------------------------------------------------------------------------------------
    // QUADTREE
    //-------------------------------------------------------------------------------------------

    // A quadtree is a data structure that partitions the space with varying degrees of resolution,
    // Useful to reduce the complexity of collision checking from O(n^2) to O(n log(n)).
    class QuadTree
    {
    public: static constexpr uint8 QT_NODE_CAPACITY = 4U;
    private:
        // Size of the region encompassed by this QuadTree.
        Vector2 Size;
        Vector2 Center;

        // TODO: Resolve issue if you have one or more colliders centered in the same place, because they
        // could cause the QuadTree to subdivide infinitely.
        //Collider **ContainedColliders;
        LinkedList<Collider*> ContainedColliders;

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
        inline bool Fits(Collider*);

        // Returns a collection of colliders the current collider may be colliding with.
        ArrayList<Collider*> Query(Collider*);

        inline QuadTree(const Vector2& _Size, const Vector2& _Center) : 
        Size(_Size), Center(_Center), NorthWest(nullptr), NorthEast(nullptr), SouthEast(nullptr), SouthWest(nullptr), ContainedColliders(LinkedList<Collider*>())
        {

        }
        inline QuadTree(Vector2&& _Size, Vector2&& _Center) : 
        Size(_Size), Center(_Center), NorthWest(nullptr), NorthEast(nullptr), SouthEast(nullptr), SouthWest(nullptr), ContainedColliders(LinkedList<Collider*>())
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

        // Subdivide
        Vector2 QuarterSize = Size / 2;
        NorthWest = new QuadTree(QuarterSize, {Center.x - QuarterSize.x / 2, Center.y + QuarterSize.y / 2});
        SouthEast = new QuadTree(QuarterSize, {Center.x + QuarterSize.x / 2, Center.y - QuarterSize.y / 2});
        NorthEast = new QuadTree(QuarterSize, Center + QuarterSize / 2);
        SouthWest = new QuadTree(QuarterSize, Center - QuarterSize / 2);

        // Move down objects

        // Check if any of the shapes is fully encompassed by one of the quadrants.

    }
};

#endif