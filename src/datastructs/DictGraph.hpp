#ifndef __DICT_GRAPH_HPP__
#define __DICT_GRAPH_HPP__

#include "../datastructs.h"
#include "Dictionary.hpp"

//-------------------------------------------------------------------------------------------
// ADJACENCY LIST GRAPH
//-------------------------------------------------------------------------------------------

template<typename T>
class DictGraph : public Graph<T>
{
private:
    Dictionary<T, LinkedList<T>> graph;
    T CurrentNode;

public:
    
};

#endif