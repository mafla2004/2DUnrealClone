#ifndef __LLMAP_HPP__
#define __LLMAP_HPP__

#include "../datastructs.h"
#include "LinkedList.hpp"

//-------------------------------------------------------------------------------------------
// MAP
//-------------------------------------------------------------------------------------------

// Slower access map, more memory efficient than a dictionary and doesn't require a Hashing Function,
// The tradeoff is access time.
template<typename K, typename V>
class Map
{
private:
    uint32 size;

    LinkedList<Pair<K,V>> keys;

public:

};

#endif