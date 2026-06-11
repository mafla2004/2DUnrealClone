#ifndef __DATASTRUCTS_H__
#define __DATASTRUCTS_H__

#include <functional>
#include <stdexcept>
#include <string>
#include <raylib.h>

// TODO: Improve following typedefs
typedef unsigned long long  uint64;
typedef unsigned int        uint32;
typedef unsigned short      uint16;
typedef unsigned char       uint8;

typedef double              Seconds;

typedef std::string         String;

template<typename T>
using HashFunc = std::function<uint16 (T)>;

#define CR_ALIAS 0
#define CR_SHALLOW 1
#define CR_DEEP 2

#define COPY_REGIMENT CR_SHALLOW

// Error message for the exception thrown by Dictionary::At if it doesn't contain a value asssociated with the key
constexpr char *ERROR_NoElementInDictionary = "Dictionary does not contain any value associated with the specified key.";

//-------------------------------------------------------------------------------------------
// COLLECTION
//-------------------------------------------------------------------------------------------

template<typename T>
class Collection
{
protected:
    uint32 size;
    
public:
    inline virtual uint32 GetSize() const final;        // Get the size of the collection, super optimized for incredible, breakneck speed (O(1) in any collection, cause we're just fetching a variable)
    virtual T& operator[](uint32)               = 0;    // Get the item at the index, it's a (JoJo) reference so you can assign it 
    virtual T operator[](uint32) const          = 0;    // Get a copy of the item at the index
    virtual void AddAt(const T&, uint32)        = 0;    // Add an item at a random location of the list, O(n) on lists and ArrayLists
    virtual void PushHead(const T&)             = 0;    // Add an item to the start (head) of the collection, quick on lists (O(1)), slow on ArrayLists (O(n))
    virtual void PushTail(const T&)             = 0;    // Append an item to the end (tail) of the collection, quick on lists (O(1)), slow on ArrayLists (O(n))
    virtual void Append(T*, uint32)             = 0;    // Append an array to a collection, quick but still O(n)
    virtual void Append(const Collection<T>&)   = 0;    // Append a collection to another collection, slow with lists (O(n^2)), faster with ArrayLists (O(n))
};

template<typename T>
inline uint32 Collection<T>::GetSize() const { return size; }

//-------------------------------------------------------------------------------------------
// ARRAY LIST
//-------------------------------------------------------------------------------------------

/*
*   So I just found out that, apparently, when it comes to templates, if you call a protected
*   field or function from a super class, it won't find it if it doesn't depend on a template
*   because the name-lookup-thingy of the compiler doesn't wait for the template to be resolved
*   and tries to look the name right away, causing an error because it doesn't find shit.
*   A way to avoid it is putting "this->" behind anything that doesn't reference a template
*   since that forces the compiler to wait.
*   Useless to say I find it stupid, suppose this is part of the reason why everyone hates C++
*   templates, but eh...
*/

template<typename T>
class ArrayList : public Collection<T>
{
private:
    T* ObjArray;
    
public:
    virtual T& operator[](uint32)               override;
    virtual T operator[](uint32) const          override;
    virtual void AddAt(const T&, uint32)        override;
    virtual void PushHead(const T&)             override;
    virtual void PushTail(const T&)             override;
    virtual void Append(T*, uint32)             override;
    virtual void Append(const Collection<T>&)   override;
    
    inline ArrayList(T* arr, uint32 dim) :
#if COPY_REGIMENT == CR_ALIAS
    ObjArray(arr) { this->size = dim };
#elif COPY_REGIMENT == CR_SHALLOW
    ObjArray(new T[dim])
    {
        this->size = dim;
        for (uint32 i = 0; i < dim; i++)
        {
            ObjArray[i] = arr[i];
        }
    }
#else // Deep copy, hard to do with templates; TODO: Implement
    , ObjArray(new T[dim])
    {
        this->size = dim;
    }
#endif
    
    // Possible memory leaks if using raw pointers, either figure out how to use pointers intelligently
    // or use C++'s smart pointers when dealing with lists.
    // Realistically though, it would be rare that a collection must destroy objects it points to, and in case
    // it would mean that it is the list itself that owns/handles them, still...
    inline ~ArrayList();
};

template<typename T>
inline ArrayList<T>::~ArrayList()
{
    delete[] ObjArray;
    ObjArray = nullptr;
    this->size = 0;
}

template<typename T>
T& ArrayList<T>::operator[](uint32 index)
{
    return ObjArray[index];
}

template<typename T>
T ArrayList<T>::operator[](uint32 index) const
{
    return ObjArray[index];
}

template<typename T>
void ArrayList<T>::AddAt(const T& Obj, uint32 index)
{ 
    // If the array is empty, we don't care about the index,
    // we care about adding the element to the first cell.
    if (!this->size)
    {
        this->size = 1U;
        ObjArray = new T[1];
        ObjArray[0] = Obj;
        return;
    }
    
    if (index > this->size)  // Since we're adding one element, position size is a valid position until we increment the size
    {
        index = this->size;
    }
    
    T* NewArr = new T[this->size + 1];
    for (uint32 i = 0U; i <= this->size; i++)
    {
        if (i == index)
        {
            NewArr[i] = Obj;
            continue;
        }
        
        if (i < index)
        {
            NewArr[i] = ObjArray[i];
            continue;
        }
        
        NewArr[i] = ObjArray[i - 1];
    }
    
    delete[] ObjArray;
    ObjArray = NewArr;
    NewArr = nullptr;
    this->size++;
}

template<typename T>
void ArrayList<T>::PushHead(const T& Obj)
{
    if (!this->size)
    {
        this->size = 1U;
        ObjArray = new T[1];
        ObjArray[0] = Obj;
        return;
    }
    
    T* NewArr = new T[this->size + 1];
    NewArr[0] = Obj;
    for (uint32 i = 1U; i <= this->size; i++)
    {
        NewArr[i] = ObjArray[i - 1];
    }
    
    delete[] ObjArray;
    ObjArray = NewArr;
    NewArr = nullptr;
    this->size++;
}

template<typename T>
void ArrayList<T>::PushTail(const T& Obj)
{
    if (!this->size)
    {
        this->size = 1U;
        ObjArray = new T[1];
        ObjArray[0] = Obj;
        return;
    }
    
    T* NewArr = new T[this->size + 1];
    NewArr[this->size] = Obj;
    for (uint32 i = 1U; i <= this->size; i++)
    {
        NewArr[i] = ObjArray[i];
    }
    
    delete[] ObjArray;
    ObjArray = NewArr;
    NewArr = nullptr;
    this->size++;
}

template<typename T>
void ArrayList<T>::Append(T* Appendee, uint32 extension)
{
    if (!Appendee || !extension)
    {
        return;
    }

    if (!this->size)
    {
        this->size = extension;
#if COPY_REGIMENT == CR_ALIAS
        ObjArray = Appendee;
#elif COPY_REGIMENT == CR_SHALLOW
        ObjArray = new T[extension];
        for (uint32 i = 0U; i < extension; i++)
        {
            ObjArray[i] = Appendee[i];
        }
#else   // Deep copy, again, hard with templates
#endif
        return;
    }

    T* NewArr = new T[this->size + extension];
    for (uint32 i = 0; i < this->size + extension; i++)
    {
        if (i < this->size)
        {
            NewArr[i] = ObjArray[i];
        }
        else
        {
            NewArr[i] = Appendee[i - this->size];
        }
    }

    delete[] ObjArray;
    ObjArray = NewArr;
    NewArr = nullptr;
    this->size += extension;
}

// TODO: Optimize by converting to array
template<typename T>
void ArrayList<T>::Append(const Collection<T>& Appendee)
{
    if (!Appendee.GetSize())
    {
        return;
    }

    if (!this->size)
    {
        this->size = Appendee.GetSize();

        ObjArray = new T[this->size];
        for (uint32 i = 0U; i < this->size; i++)
        {
            ObjArray[i] = Appendee[i];
        }

        return;
    }

    T* NewArr = new T[this->size + Appendee.GetSize()];
    for (uint32 i = 0U; i < this->size + Appendee.GetSize(); i++)
    {
        if (i < this->size)
        {
            NewArr[i] = ObjArray[i];
        }
        else
        {
            NewArr[i] = Appendee[i - this->size];
        }
    }

    delete[] ObjArray;
    ObjArray = NewArr;
    NewArr = nullptr;
    this->size += Appendee.GetSize();
}

//-------------------------------------------------------------------------------------------
// DOUBLE LINKED LIST
//-------------------------------------------------------------------------------------------

// Helper node class
template<typename T>
class DoubleLinkedNode
{
//friend class LinkedList;
private:
    T info;
    DoubleLinkedNode<T> *next, *previous;

public:
    inline T& GetInfo();
    inline T GetInfo() const;
    inline DoubleLinkedNode<T>* GetNext() const;
    inline DoubleLinkedNode<T>* GetPrevious() const;
    inline void SetNext(DoubleLinkedNode<T>*);
    inline void SetPrevious(DoubleLinkedNode<T>*);

    // Here we can avoid use of pointer this because all three fields depend on the template
    inline DoubleLinkedNode(
        const T& item, 
        DoubleLinkedNode<T> *n = nullptr, 
        DoubleLinkedNode<T> *p = nullptr
    ) : info(item), next(n), previous(p) {}
};

template<typename T>
inline T& DoubleLinkedNode<T>::GetInfo() { return info; }
template<typename T>
inline T DoubleLinkedNode<T>::GetInfo() const { return info; }
template<typename T>
inline DoubleLinkedNode<T>* DoubleLinkedNode<T>::GetNext() const { return next; }
template<typename T>
inline DoubleLinkedNode<T>* DoubleLinkedNode<T>::GetPrevious() const { return previous; }
template<typename T>
inline void DoubleLinkedNode<T>::SetNext(DoubleLinkedNode<T>* n) { next = n; }
template<typename T>
inline void DoubleLinkedNode<T>::SetPrevious(DoubleLinkedNode<T>* p) { previous = p; }

template<typename T>
class ListIterator
{
private:
    DoubleLinkedNode<T> *current;
public:
    inline T& operator*();
    inline ListIterator<T>& operator++();
    inline ListIterator<T>& operator--();
    inline ListIterator<T>  operator++(int);
    inline ListIterator<T>  operator--(int);
    inline bool operator==(const ListIterator<T>&);
    inline bool operator!=(const ListIterator<T>&);

    inline ListIterator(DoubleLinkedNode<T> *init) : current(init) {}
    inline ListIterator(const ListIterator<T>& other) : current(other.current) {}
};

template<typename T>
inline T& ListIterator<T>::operator*() { return current->GetInfo(); }
// This is something worthy of a programming horror compilation
template<typename T>
inline bool ListIterator<T>::operator==(const ListIterator<T>& other) { return current == other.current; }
template<typename T>
inline bool ListIterator<T>::operator!=(const ListIterator<T>& other) { return current != other.current; }

template<typename T>
inline ListIterator<T>& ListIterator<T>::operator++() 
{
    if (current && current->GetNext())
    {
        current = current->GetNext();
    }

    return *this;
}

template<typename T>
inline ListIterator<T>& ListIterator<T>::operator--() 
{
    if (current && current->GetPrevious())
    {
        current = current->GetPrevious();
    }

    return *this;
}

template<typename T>
inline ListIterator<T> ListIterator<T>::operator++(int)
{
    ListIterator TempIter = *this;
    ++*this;
    return TempIter;
}

template<typename T>
inline ListIterator<T> ListIterator<T>::operator--(int)
{
    ListIterator<T> TempIter = *this;
    --*this;
    return TempIter;
}

// LinkedList class, doubly linked.
template<typename T>
class LinkedList : public Collection<T>
{
private:
    DoubleLinkedNode<T> *head, *tail;

public:
    virtual T& operator[](uint32)               override;
    virtual T  operator[](uint32) const         override;
    virtual void AddAt(const T&, uint32)        override;
    virtual void PushHead(const T&)             override;
    virtual void PushTail(const T&)             override;
    virtual void Append(T*, uint32)             override;
    virtual void Append(const Collection<T>&)   override;

    inline ListIterator<T> begin();
    inline ListIterator<T> end();

    inline LinkedList(const T& item) : head(new DoubleLinkedNode<T>(item)), tail(head)
    {
        this->size = 1U;
    }
    inline LinkedList() : head(nullptr), tail(nullptr) { this->size = 0U; }
};

template<typename T>
T& LinkedList<T>::operator[](uint32 index)
{
    if (!index)
    {
        return head->GetInfo();
    }

    if (index >= this->size - 1)
    {
        return tail->GetInfo();
    }

    DoubleLinkedNode<T> *iter;

    if (index >= this->size / 2)
    { iter = tail;
        for (uint32 i = this->size - 1; i > index; i--)
        {
            // "Why don't we do null checks?"
            // Because I want the program to crash.
            // If we get a null here, something seriously fucked up went on,
            // we don't need a safeguard for it because the program must not keep
            // executing if it happens, if it happens I get back here and fix this shit.
            
            iter = iter->GetPrevious();
        }
        return iter->GetInfo();
    }

    iter = head;
    for (uint32 i = 0; i < index; i++)
    {
        iter = iter->GetNext();
    }
    return iter->GetInfo();
}

template<typename T>
T LinkedList<T>::operator[](uint32 index) const
{
    if (!index)
    {
        return head->GetInfo();
    }

    if (index >= this->size - 1)
    {
        return tail->GetInfo();
    }

    DoubleLinkedNode<T> *iter;

    if (index >= this->size / 2)
    { iter = tail;
        for (uint32 i = this->size - 1; i > index; i--)
        {
            iter = iter->GetPrevious();
        }
        return iter->GetInfo();
    }

    iter = head;
    for (uint32 i = 0; i < index; i++)
    {
        iter = iter->GetNext();
    }
    return iter->GetInfo();
}

template<typename T>
void LinkedList<T>::AddAt(const T& item, uint32 index)
{
    DoubleLinkedNode<T> *NewNode = new DoubleLinkedNode<T>(item);
    if (!this->size)
    {
        head = tail = NewNode;
        this->size = 1U;
        return;
    }

    this->size++;

    if (!index)
    {
        NewNode->SetNext(head);
        head->SetPrevious(NewNode);
        head = NewNode;
        return;
    }

    if (index > this->size)
    {
        NewNode->SetPrevious(tail);
        tail->SetNext(NewNode);
        tail = NewNode;
        return;
    }

    DoubleLinkedNode<T> *iter = head;
    for (uint32 i = 0; i < index; i++)
    {
        iter = iter->GetNext();
    }
    iter->GetNext()->SetPrevious(NewNode);
    NewNode->SetNext(iter->GetNext());
    NewNode->SetPrevious(iter);
    iter->SetNext(NewNode);
}

template<typename T>
void LinkedList<T>::PushHead(const T& item)
{
    DoubleLinkedNode<T> *NewNode = new DoubleLinkedNode<T>(item);
    if (!this->size)
    {
        head = tail = NewNode;
        this->size = 1U;
        return;
    }

    NewNode->SetNext(head);
    head->SetPrevious(NewNode);
    head = NewNode;
}

template<typename T>
void LinkedList<T>::PushTail(const T& item)
{
    DoubleLinkedNode<T> *NewNode = new DoubleLinkedNode<T>(item);
    if (!this->size)
    {
        head = tail = NewNode;
        this->size = 1U;
        return;
    }

    NewNode->SetPrevious(tail);
    tail->SetNext(NewNode);
    tail = NewNode;
}

template<typename T>
void LinkedList<T>::Append(T* Appendee, uint32 extension)
{
    if (!Appendee || !extension)
    {
        return;
    }

    DoubleLinkedNode<T> *AppendedNode;
    for(uint32 i = 0; i < extension; i++)
    {
        AppendedNode = new DoubleLinkedNode<T>(Appendee[i], nullptr, tail);
        tail->SetNext(AppendedNode);
        tail = AppendedNode;
    }

    this->size += extension;
}

template<typename T>
void LinkedList<T>::Append(const Collection<T>& Appendee)
{
    if (!Appendee.GetSize())
    {
        return;
    }

    this->size += Appendee.GetSize();

    // A bit of a nasty hack to check if the collection is a LinkedList,
    // that way appending it can be faster (O(1))
    LinkedList<T> *AsLinkedList = (LinkedList<T>*)(&Appendee);
    if (AsLinkedList)
    {
        tail->SetNext(AsLinkedList->head);
        AsLinkedList->head->SetPrevious(tail);
        return;
    }

    // This is why I made the nasty hack above.
    // Appending using a for loop yields O(n * cost_of_random_access) complexity,
    // and since LinkedList have O(n) cost of random access, the complexity becomes
    // O(n^2), but due to the recursivity of LinkedLists, appending one LinkedList to
    // another is O(1), that nasty play with pointers was to check whether or not
    // the collection was a LinkedList (and get a null pointer if it isn't)
    DoubleLinkedNode<T> *AppendedNode;
    for (uint32 i = 0; i < Appendee.GetSize(); i++)
    {
        AppendedNode = new DoubleLinkedNode<T>(Appendee[i], nullptr, tail);
        tail->SetNext(AppendedNode);
    }
}

template<typename T>
inline ListIterator<T> LinkedList<T>::begin() { return ListIterator<T>(head); }
template<typename T>
inline ListIterator<T> LinkedList<T>::end() { return ListIterator<T>(tail); }

//-------------------------------------------------------------------------------------------
// MAP
//-------------------------------------------------------------------------------------------

// Slower access map, more memoru efficient than a dictionary and doesn't require a Hashing Function,
// The tradeoff is access time.
template<typename K, typename V>
class Map
{
private:
    uint32 size;

    LinkedList<K> keys;
    LinkedList<V> values;

public:

};

//-------------------------------------------------------------------------------------------
// PAIR - used in many data structures
//-------------------------------------------------------------------------------------------

template<typename K, typename V>
class Pair
{
private:
    const K key;
    V value;
public:
    inline const K& GetKey() const;

    // This is bad, fix
    inline V& GetValue();

    inline void SetValue(const V&);

    inline Pair(const K& _key) : key(_key), value({}) {}
    inline Pair(const K& _key, const V& _value) : key(_key), value(_value) {}
    
    // TODO: LEARN HOW TO MAKE DESTRUCTORS YOU FUCKHEAD!!! Aka. What if the type stored are pointers, does
    // the default destructor 
    ~Pair() = default;
};

template<typename K, typename V>
inline const K& Pair<K,V>::GetKey() const { return key; }
template<typename K, typename V>
inline V& Pair<K,V>::GetValue() { return value; }
template<typename K, typename V>
inline void Pair<K,V>::SetValue(const V& val) { value = val; }


//-------------------------------------------------------------------------------------------
// DICTIONARY
//-------------------------------------------------------------------------------------------

// Dictionary, also known as HashMap, very quick random access, requires a hash function to be fed in
// at construction time
template<typename K, typename V>
class Dictionary
{
private:

    HashFunc<K> HashingFunction;
    uint32 count;
    uint16 size;

    LinkedList<Pair<K, V>>* Buckets;

    inline uint16 GetIndex(const K&);

public:
    inline uint32 GetCount();                   // Gets the amount of items memorized in the Dictionary
    inline uint16 GetSize();                    // Gets the size of the dictionary, not the amount of items put into it, but the amount of buckets it contains, it is 256 by default
    
    inline bool Contains(const K&);
    inline bool Insert(const K&, const V&);     // Inserts the value at the key, only if the dictionary doesn't already contain something associated with this key
    
    inline V* PtrTo(const K&) noexcept;         // Returns a pointer to the value associated with the key, returns null if none is found
    inline V& At(const K&);                     // Returns a reference to the value associated with the key, throws an exception if none is found

    inline V& operator[](const K&);             // Returns the object associated with the value, creating a new one if none exists
    //inline const V const* operator[](uint16) const; This is a collection made for quick access & modification, why the FUCK would I need a const func bruh

    // Because I'm a FUCKING GENIUS I figured out how to handle the hashing problem...
    // You just ask for a hashfunc at construction time :3
    // HashFunc<K> is an alias of std::function<uint16 (K)>
    Dictionary(const HashFunc<K>& hashing, uint16 _size = 256U) : HashingFunction(hashing), size(_size), count(0U), Buckets(new LinkedList<Pair<K,V>>[size]) {}
};

template<typename K, typename V>
inline uint32 Dictionary<K,V>::GetCount() { return count; }
template<typename K, typename V>
inline uint16 Dictionary<K,V>::GetSize() { return size; }
template<typename K, typename V>
inline uint16 Dictionary<K,V>::GetIndex(const K& key) { return HashingFunction(key) % size; }

// Do I have a clue what noexcept does other than signal to exception is to come from here?
// Nope :3
// Am I putting it here only because another similar method will throw an exception?
// Yep :3
template<typename K, typename V>
inline V* Dictionary<K,V>::PtrTo(const K& key) noexcept
{
    uint16 BucketIndex = GetIndex(key);
    LinkedList<Pair<K,V>>& bucket = Buckets[BucketIndex];

    if (!bucket.GetSize())
    {
        return nullptr;
    }

    for (const Pair<K, V>& pair : bucket)
    {
        if (pair.GetKey() == key)
            return &(pair.GetValue());
    }

    return nullptr;
}

template<typename K, typename V>
inline V& Dictionary<K,V>::At(const K& key)
{
    uint16 BucketIndex = GetIndex(key);
    LinkedList<Pair<K,V>>& bucket = Buckets[BucketIndex];

    if (!bucket.GetSize())
    {
        throw std::out_of_range(ERROR_NoElementInDictionary);
    }

    for (const Pair<K, V>& pair : bucket)
    {
        if (pair.GetKey() == key)
            return pair.GetValue();
    }

    throw std::out_of_range(ERROR_NoElementInDictionary);
}

template<typename K, typename V>
inline V& Dictionary<K,V>::operator[](const K& key)
{
    uint16 BucketIndex = GetIndex(key);
    LinkedList<Pair<K,V>>& bucket = Buckets[BucketIndex];
    // So uhh, funny little bug I found on the list above, if I didn't specify a reference to LinkedList, the line would create a copy
    // and then add the value to the copy which resulted in the Dictionary being unable to add anything to its actual buckets because
    // operations like PushHead would happen on the copy of the bucket rather than on the bucket itself... funny :3

    if (!bucket.GetSize())
    {
        bucket.PushHead({key});
        this->count++;
        return bucket[0].GetValue();
    }

    for (Pair<K, V>& pair : bucket)
    {
        if (pair.GetKey() == key)
            return pair.GetValue();
    }

    bucket.PushTail({key});
    this->count++;
    return bucket[bucket.GetSize() - 1].GetValue(); 
    // "But we scan the entire list for-" no we don't, I coded the op overload to access immediately to the tail if the index is greater or equal to the last
    // index, I ain't dumb, calm your tits if you have any, your balls otherwise
}

template<typename K, typename V>
bool Dictionary<K,V>::Contains(const K& key)
{
    uint16 BucketIndex = GetIndex(key);

    if (!Buckets[BucketIndex].GetSize())
    {
        return false;
    }

    for (const Pair<K, V>& pair : Buckets[BucketIndex])
    {
        if (pair.GetKey() == key)
            return true;
    }

    return false;
}

template<typename K, typename V>
bool Dictionary<K,V>::Insert(const K& key, const V& value)
{
    uint16 BucketIndex = GetIndex(key);
    LinkedList<Pair<K,V>>& bucket = Buckets[BucketIndex];

    if (!bucket.GetSize())
    {
        this->count++;
        bucket->PushHead({key, value});
        return true;
    }

    for (const Pair<K, V>& pair : Buckets[BucketIndex])
    {
        if (pair.GetKey() == key)
            return false;
    }

    this->count++;
    bucket->PushTail({key, value});
    return true;
}

//-------------------------------------------------------------------------------------------
// GRAPH ABSTRACT CLASS
//-------------------------------------------------------------------------------------------

template<typename T>
class Graph 
{
private:
    uint32 NodeCount;
    uint32 ArcCount;

    bool Oriented;
public:
    virtual inline uint32 GetNodeCount()    final;
    virtual inline uint32 GetArcCount()     final;
    
    virtual inline bool IsOriented() final; // Returns whether or not the graph is oriented (if oriented, an arc that conects node A to B may not connect B to A)
};

template<typename T>
inline uint32 Graph<T>::GetNodeCount() { return NodeCount; }
template<typename T>
inline uint32 Graph<T>::GetArcCount() { return ArcCount; }
template<typename T>
inline bool Graph<T>::IsOriented() { return Oriented; }

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