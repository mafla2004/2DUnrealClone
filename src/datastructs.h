#ifndef __DATASTRUCTS_H__
#define __DATASTRUCTS_H__

#include <raylib.h>

// TODO: Improve following typedefs
typedef unsigned long long  uint64;
typedef unsigned int        uint32;
typedef unsigned short      uint16;
typedef unsigned char       uint8;

typedef double              Seconds;

#define CR_ALIAS 0
#define CR_SHALLOW 1
#define CR_DEEP 2

#define COPY_REGIMENT CR_SHALLOW

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

    inline LinkedList(const T& item) : head(new DoubleLinkedNode<T>(item)), tail(head)
    {
        this->size = 1U;
    }
};

//-------------------------------------------------------------------------------------------
// GRAPH ABSTRACT CLASS
//-------------------------------------------------------------------------------------------

template<typename T>
class Graph 
{
public:
    
};

#endif