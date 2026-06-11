#include "datastructs.h"

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