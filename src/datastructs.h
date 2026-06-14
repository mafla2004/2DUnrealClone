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

    inline Pair(const K& _key) : key(_key), value() {}
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

#endif