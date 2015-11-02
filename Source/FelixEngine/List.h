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
#include "Semaphore.h"


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
    struct Node
    {
      T item;
      Node *next, *prev;
    };
    static Pool<Node> NodePool;
  
  public:
    class Iterator
    {
    public:
      Iterator(Node *node = nullptr): mNode(0) {setNode(node);}
      Iterator(const Iterator &other): mNode(0) {*this = other;}
      ~Iterator() {}
      
      Iterator& operator=(const Iterator &other)
      {
        setNode(other.mNode);
        return *this;
      }
      Iterator& operator=(Node *node)
      {
        setNode(node);
        return *this;
      }
      
      bool operator==(const Iterator &other) const {return mNode == other.mNode;}
      bool operator==(const Node *node) const {return mNode == node;}
      
      bool operator!=(const Iterator &other) const {return mNode != other.mNode;}
      bool operator!=(const Node *node) const {return mNode != node;}
      
      T& operator*() {return mNode->item;}
      
      Iterator& operator++()
      {
        if (mNode)
          setNode(mNode->next);
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
        if (mNode)
          setNode(mNode->prev);
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
      void setNode(Node *node)
      {
        mNode = node;
      }
      
      Node *mNode;
    };
    
  public:
    List() {init();}
    List(const List &other)
    {
      init();
      *this = other;
    }
    ~List()
    {
      Node *node = mRoot->next;
      while (node != mRoot)
      {
        node = node->next;
        NodePool.freeItem(node->prev);
      }
      NodePool.freeItem(mRoot);
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
    
    Iterator begin() const {return mRoot->next;}
    Iterator end() const {return mRoot;}
    
    int size() const {return SDL_AtomicGet(&mSize);}
    
    void pushFront(const T &item) {insert(begin(), item);}
    void pushBack(const T &item) {insert(end(), item);}
    void insert(const Iterator &itr, const T &item)
    {
      Node *node = NodePool.newItem();
      node->item = item;
      if (itr == mRoot)
      {
        node->next = mRoot;
        node->prev = mRoot->prev;
        mRoot->prev->next = node;
        mRoot->prev = node;
      }
      else
      {
        node->next = itr.mNode->next;
        node->prev = itr.mNode;
        itr.mNode->next->prev = node;
        itr.mNode->next = node;
      }
      SDL_AtomicAdd(&mSize, 1);
    }
    
    void pushFrontSafe(const T &item) {insertSafe(begin(), item);}
    void pushBackSafe(const T &item) {insertSafe(end(), item);}
    void insertSafe(const Iterator &itr, const T &item)
    {
      lock();
      insert(itr, item);
      unlock();
    }
    
    void popFront() {remove(begin());}
    void popBack() {remove(end());}
    void clear()
    {
      while (mRoot != mRoot->next)
        remove(mRoot->next);
    }
    Iterator remove(const Iterator &itr)
    {
      Node *next = nullptr;
      if (itr.mNode)
      {
        if (itr.mNode == mRoot)
          next = mRoot;
        else
        {
          next = itr.mNode->next;
          next->prev = itr.mNode->prev;
          itr.mNode->prev->next = next;
          NodePool.freeItem(itr.mNode);
          SDL_AtomicAdd(&mSize, -1);
        }
      }
      return next;
    }
    
    void popFrontSafe() {removeSafe(begin());}
    void popBackSafe() {removeSafe(end());}
    Iterator removeSafe(const Iterator &itr)
    {
      lock();
      remove(itr);
      unlock();
    }
    void clearSafe()
    {
      lock();
      clear();
      unlock();
    }
    
    void wait() {mSem.wait();}
    void post() {mSem.post();}
    
    void lock() {mSem.lock();}
    void unlock() {mSem.unlock();}
    
    static void CleanUpPool() {NodePool.cleanUp();}
  private:
    void init()
    {
      SDL_AtomicSet(mSize, 0);
      mRoot = NodePool.newItem();
      mRoot->next = mRoot->prev = mRoot;
    }
    
    Node *mRoot;
    SDL_atomic_t mSize;
    Semaphore mSem;
  };
  
  template <typename T>
  Pool<typename List<T>::Node> List<T>::NodePool(DEFAULT_LIST_POOL_SIZE);
}


#endif /* List_h */
