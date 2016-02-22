//
//  OwnPtr.h
//  FelixEngine
//
//  Created by Robert Crosby on 1/14/16.
//  Copyright © 2016 Robert Crosby. All rights reserved.
//

#ifndef OwnPtr_h
#define OwnPtr_h

#include <iostream>


namespace fx
{
  /**
   * Smart Pointer that either retains or shares ownership
   * of a dynamicly allocated object.
   */
  template <typename T>
  class OwnPtr
  {
  public:
    /**
     * Constructor either default constructs the object or copies from the given refrence.
     */
    OwnPtr(const T &val = T()): mPtr(0), mHasOwnership(0) {*this = val;}
    
    /**
     * Constructor initalizes to the given shared pointer.
     */
    OwnPtr(T *ptr): mPtr(ptr), mHasOwnership(0) {}
    
    /**
     * Copy Consturtor.
     */
    OwnPtr(const OwnPtr &ptr): mPtr(0), mHasOwnership(0) {*this = ptr;}
    
    /**
     * Destructor clears the pointer.
     */
    ~OwnPtr() {clear();}
    
    /**
     * Refrence Assignment copies the reference to a localy owned object.
     */
    OwnPtr& operator=(const T &val)
    {
      if (&val != mPtr)
      {
        clear();
        mHasOwnership = true;
        mPtr = new T(val);
      }
      return *this;
    }
    
    /**
     * Pointer Assignment sets a shared pointer.
     */
    OwnPtr& operator=(T *ptr)
    {
      if (mPtr != ptr)
        clear();
      mHasOwnership = false;
      mPtr = ptr;
      return *this;
    }
    
    /**
     * Assignment Copies either the shared pointer or the locally owned object.
     */
    OwnPtr& operator=(const OwnPtr &ptr)
    {
      if (this != &ptr)
        return ptr.mHasOwnership ? *this = *ptr.mPtr : *this = ptr.mPtr;
      return *this;
    }
    
    /**
     * Deletes the pointer if has ownership and assigns the pointer to null.
     */
    void clear()
    {
      if (mPtr && mHasOwnership)
        delete mPtr;
      mPtr = nullptr;
    }
    
    T* operator->() {return mPtr;}
    T& operator*() {return *mPtr;}
    const T& operator*() const {return *mPtr;}
    
    T* ptr() const {return mPtr;}
    
  private:
    bool mHasOwnership;
    T *mPtr;
  };
}


#endif /* OwnPtr_h */
