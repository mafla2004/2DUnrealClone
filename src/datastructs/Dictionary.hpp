#ifndef __DICT_HPP__
#define __DICT_HPP__

#include "../datastructs.h"
#include "LinkedList.hpp"

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

#endif