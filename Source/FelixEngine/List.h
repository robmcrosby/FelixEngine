//
//  List.h
//  FelixEngine
//
//  Created by Robert Crosby on 11/1/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef List_h
#define List_h

#include "Pool.h"
#include "Mutex.h"


#define DEFAULT_LIST_POOL_SIZE 64

namespace fx
{
  /**
   *
   */
  template <typename T>
  class List
  {
  private:
    class Node
    {
    public:
      Node() {init();}
      void init(const T &item = T())
      {
        mItem = item;
        mCount = mRemove = 0;
        mNext = mPrev = this;
      }
      bool remove()
      {
        if (mRemove && !mCount)
        {
          mPrev->mNext = mNext;
          mNext->mPrev = mPrev;
          return true;
        }
        return false;
      }
      void insertAfter(Node *node)
      {
        mNext = node->mNext;
        mPrev = node;
        node->mNext->mPrev = this;
        node->mNext = this;
      }
      
      const T& item() const {return mItem;}
      
      int operator++() {return ++mCount;}
      int operator--() {return --mCount;}
      int count() const {return mCount;}
      
      void setRemove() {mRemove = 1;}
      bool removed() const {return mRemove;}
      
      Node* next() const {return mNext;}
      Node* prev() const {return mPrev;}
      
    private:
      T mItem;
      int mCount, mRemove;
      Node *mNext, *mPrev;
    };
    static Pool<Node> NodePool;
  
  public:
    class Iterator
    {
    public:
      Iterator(List *list, Node *node): mList(list), mNode(0) {*this = node;}
      Iterator(const Iterator &other): mList(other.mList), mNode(0) {*this = other;}
      ~Iterator()
      {
        if (mList)
          mList->releaseNode(&mNode);
      }
      
      Iterator& operator=(const Iterator &other) {return *this = other.mNode;}
      Iterator& operator=(Node *node)
      {
        if (mList)
          mList->assignNode(&mNode, node);
        return *this;
      }
      
      bool operator==(const Iterator &other) const {return mNode == other.mNode;}
      bool operator==(const Node *node) const {return mNode == node;}
      
      bool operator!=(const Iterator &other) const {return mNode != other.mNode;}
      bool operator!=(const Node *node) const {return mNode != node;}
      
      const T& operator*() const {return mNode->item();}
      
      Iterator& operator++()
      {
        if (mList)
        {
          do {
            mList->incrementNode(&mNode);
          } while (mNode->removed());
        }
        return *this;
      }
      Iterator operator++(int)
      {
        Iterator tmp = *this;
        ++*this;
        return tmp;
      }
      Iterator& operator--()
      {
        if (mList)
        {
          do {
            mList->decrementNode(&mNode);
          } while (mNode->removed());
        }
        return *this;
      }
      Iterator operator--(int)
      {
        Iterator tmp = *this;
        --*this;
        return tmp;
      }
      
    private:
      friend List;
      
      Node *mNode;
      List *mList;
    };
    
  public:
    List(): mLock(0) {SDL_AtomicSet(&mSize, 0);}
    List(const List &other)
    {
      SDL_AtomicSet(&mSize, 0);
      *this = other;
    }
    ~List()
    {
      Node *node = mRoot.next();
      while (node != &mRoot)
      {
        node = node->next();
        NodePool.freeItem(node->prev());
      }
    }
    
    List& operator=(const List &other)
    {
      if (&other != this)
      {
        clear();
        for (Iterator itr = other.begin(); itr != other.end(); ++itr)
          pushBack(*itr);
      }
      return *this;
    }
    
    Iterator begin() {return ++Iterator(this, &mRoot);}
    Iterator end() {return Iterator(this, &mRoot);}
    
    int size() const {return SDL_AtomicGet(&mSize);}
    
    void pushFront(const T &item) {insert(end(), item);}
    void pushBack(const T &item) {insert(--end(), item);}
    void insert(const Iterator &itr, const T &item) {insertNode(itr.mNode, item);}
    
    void popFront()
    {
      Iterator itr = begin();
      while (!removeNode(itr.mNode))
        ++itr;
    }
    void popBack()
    {
      Iterator itr = --end();
      while (!removeNode(itr.mNode))
        --itr;
    }
    Iterator remove(Iterator itr)
    {
      removeNode(itr.mNode);
      return ++itr;
    }
    
    bool remove(const T &item)
    {
      bool success = false;
      Iterator itr = begin();
      while (itr != end())
      {
        if (*itr == item)
        {
          itr = remove(itr);
          success = true;
        }
        else
          ++itr;
      }
      return success;
    }

    void clear()
    {
      Iterator itr = begin();
      while (itr != end())
        itr = remove(itr);
    }
    
    static void CleanUpPool() {NodePool.cleanUp();}
  private:
    friend Iterator;
    
    void releaseNode(Node **ref)
    {
      SDL_AtomicLock(&mLock);
      if (*ref)
      {
        --(**ref);
        if ((*ref)->remove())
          NodePool.freeItem(*ref);
        *ref = nullptr;
      }
      SDL_AtomicUnlock(&mLock);
    }
    void assignNode(Node **ref, Node *node)
    {
      SDL_AtomicLock(&mLock);
      if (*ref)
      {
        --(**ref);
        if ((*ref)->remove())
          NodePool.freeItem(*ref);
      }
      *ref = node;
      if (*ref)
        ++(**ref);
      SDL_AtomicUnlock(&mLock);
    }
    void incrementNode(Node **ref)
    {
      SDL_AtomicLock(&mLock);
      if (*ref)
      {
        Node *next = (*ref)->next();
        --(**ref);
        if ((*ref)->remove())
          NodePool.freeItem(*ref);
        *ref = next;
        if (*ref)
          ++(**ref);
      }
      SDL_AtomicUnlock(&mLock);
    }
    void decrementNode(Node **ref)
    {
      SDL_AtomicLock(&mLock);
      if (*ref)
      {
        Node *prev = (*ref)->prev();
        --(**ref);
        if ((*ref)->remove())
          NodePool.freeItem(*ref);
        *ref = prev;
        if (*ref)
          ++(**ref);
      }
      SDL_AtomicUnlock(&mLock);
    }
    bool removeNode(Node *ptr)
    {
      bool success = true;
      
      SDL_AtomicLock(&mLock);
      if (ptr)
      {
        if (ptr == &mRoot && mRoot.prev() != &mRoot)
          ptr = mRoot.prev();
        if (ptr != &mRoot)
        {
          if (ptr->removed())
            success = false;
          else
            ptr->setRemove();
        }
      }
      SDL_AtomicUnlock(&mLock);
      return success;
    }
    void insertNode(Node *ptr, const T &item)
    {
      Node *node = NodePool.newItem();
      node->init(item);
      
      SDL_AtomicLock(&mLock);
      node->insertAfter(ptr);
      SDL_AtomicAdd(&mSize, 1);
      SDL_AtomicUnlock(&mLock);
    }
    
    Node mRoot;
    SDL_SpinLock mLock;
    SDL_atomic_t mSize;
  };
  
  template <typename T>
  Pool<typename List<T>::Node> List<T>::NodePool(DEFAULT_LIST_POOL_SIZE);
}


#endif /* List_h */
