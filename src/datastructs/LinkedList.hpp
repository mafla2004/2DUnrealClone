#ifndef __LINKED_LIST_HPP__
#define __LINKED_LIST_HPP__

#include "../datastructs.h"

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
    inline bool operator==(const ListIterator<T>&) const;
    inline bool operator!=(const ListIterator<T>&) const;

    inline ListIterator(DoubleLinkedNode<T> *init) : current(init) {}
    inline ListIterator(const ListIterator<T>& other) : current(other.current) {}
};

template<typename T>
inline T& ListIterator<T>::operator*() { return current->GetInfo(); }
template<typename T>
inline bool ListIterator<T>::operator==(const ListIterator<T>& other) const { return current == other.current; }
template<typename T>
inline bool ListIterator<T>::operator!=(const ListIterator<T>& other) const { return current != other.current; }

template<typename T>
inline ListIterator<T>& ListIterator<T>::operator++() 
{
    if (current)
    {
        current = current->GetNext();
    }

    return *this;
}

template<typename T>
inline ListIterator<T>& ListIterator<T>::operator--() 
{
    if (current)
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
    DoubleLinkedNode<T> *head = nullptr, *tail = nullptr;

public:
    virtual T& operator[](uint32)               override;
    virtual T  operator[](uint32) const         override;
    virtual void AddAt(const T&, uint32)        override;
    virtual void PushHead(const T&)             override;
    virtual void PushTail(const T&)             override;
    virtual void Append(T*, uint32)             override;
    virtual void Append(const Collection<T>&)   override;

    inline ListIterator<T> begin();
    inline ListIterator<T> last();  // Returns an iterator pointing to the last element of the list
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
    this->size++;
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
    this->size++;
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
inline ListIterator<T> LinkedList<T>::last() { return ListIterator<T>(tail); }
template<typename T>
inline ListIterator<T> LinkedList<T>::end() { return ListIterator<T>(nullptr); }

#endif