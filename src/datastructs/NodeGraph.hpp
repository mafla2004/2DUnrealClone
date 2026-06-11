#ifndef __NODE_GRAPH_HPP__
#define __NODE_GRAPH_HPP__

#include "datastructs.h"
#include "LinkedList.hpp"

//-------------------------------------------------------------------------------------------
// NODE GRAPH
//-------------------------------------------------------------------------------------------

// Node-based graph class, implements a state-machine-like graph in the most straightforward way.
template<typename T>
class NodeGraph : public Graph<T>
{
private:
    struct GraphNode
    {
    private:
        T info;
        LinkedList<GraphNode*> connections;
        
    public:
        inline T GetInfo() const;
        
        inline void SetInfo(const T&);
        
        inline GraphNode(const T& _info, Collection<GraphNode*>* _cnct = nullptr, bool Oriented = false) : info(_info), connections(LinkedList<T>())
        {
            if (!_cnct)
            {
                return;
            }

            for (GraphNode* i : _cnct)
            {
                connections.PushTail(i);

                if (!Oriented) continue;

                i->connections.PushTail(this);
            }
        }
    };

    
};

template<typename T>
inline T NodeGraph<T>::GraphNode::GetInfo() const { return info; }
template<typename T>
inline void NodeGraph<T>::GraphNode::SetInfo(const T& newInfo) { info = newInfo; }

#endif