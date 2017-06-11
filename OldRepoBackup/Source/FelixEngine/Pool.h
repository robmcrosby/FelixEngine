//
//  Pool.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/29/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Pool_h
#define Pool_h

#include "Semaphore.h"

#define DEFAULT_POOL_SIZE 64


namespace fx
{
  template <typename T>
  class Block
  {
  public:
    Block(int size): mSem(size), mNext(0) {mData = new T[size];}
    ~Block()
    {
      delete [] mData;
      delete mNext;
    }
    
    T* getItem()
    {
      int index = mSem.tryWaitForIndex();
      return index < 0 ? nullptr : mData+index;
    }
    bool setItem(T *item)
    {
      if (mData <= item && item < mData+mSem.size())
      {
        int index = (int)(item - mData);
        mSem.postIndex(index);
        return true;
      }
      return false;
    }
    
    Block* next() const {return mNext;}
    void addNextBlock()
    {
      if (!mNext)
        mNext = new Block(mSem.size());
    }
    void removeNextBlock()
    {
      if (mNext)
      {
        Block *block = mNext;
        mNext = block->mNext;
        block->mNext = nullptr;
        delete block;
      }
    }
    bool inUse() const {return mSem.value() != mSem.size();}
    
  private:
    T *mData;
    IndexedSemaphore mSem;
    Block *mNext;
  };
  
  
  template <typename T>
  class Pool
  {
  public:
    Pool(int size = DEFAULT_POOL_SIZE): mHead(size) {}
    
    T* newItem()
    {
      T *item = nullptr;
      Block<T> *block = &mHead;
      
      mSem.wait();
      while (!item)
      {
        item = block->getItem();
        if (!item)
        {
          if (!block->next())
          {
            // Lock the Semaphore
            mSem.post();
            mSem.lock();
            
            // Attempt to add the new block
            block->addNextBlock();
            
            // Unlock the Semaphore
            mSem.unlock();
            mSem.wait();
          }
          block = block->next();
        }
      }
      mSem.post();
      return item;
    }
    void freeItem(T *item)
    {
      mSem.wait();
      Block<T> *block = &mHead;
      while (block)
      {
        if (block->setItem(item))
          block = nullptr;
        else
          block = block->next();
      }
      mSem.post();
    }
    void cleanUp()
    {
      mSem.lock();
      Block<T> *block = &mHead;
      while (block->next)
      {
        if (!block->next->inUse())
          block->removeNextBlock();
        else
          block = block->next;
      }
      mSem.unlock();
    }
    
  private:
    Block<T> mHead;
    Semaphore mSem;
  };
}


#endif /* Pool_h */
