#include "datastructs.h"
#include <iostream>
/*
//-------------------------------------------------------------------------------------------
// ARRAY LIST
//-------------------------------------------------------------------------------------------

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
// LINKED LIST
//-------------------------------------------------------------------------------------------

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
        std::cout << "Appending Linked List" << std::endl;
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
}*/