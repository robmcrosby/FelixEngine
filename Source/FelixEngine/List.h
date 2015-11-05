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
    struct Node
    {
      void init()
      {
        next = prev = nullptr;
      }
      
      T item;
      Node *next, *prev;
    };
    static Pool<Node> NodePool;
  
  public:
    class Iterator
    {
    public:
      Iterator(List *list, Node *node): mList(list), mNode(0) {*this = node;}
      Iterator(const Iterator &other): mList(other.mList), mNode(0) {*this = other;}
      ~Iterator() {}
      
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
      
      const T& operator*() const {return mNode->item;}
      
      Iterator& operator++()
      {
        if (mList)
          mList->incrementNode(&mNode);
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
          mList->decrementNode(&mNode);
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
    
    Iterator begin() {return ++Iterator(this, mRoot);}
    Iterator end() {return Iterator(this, mRoot);}
    
    int size() const {return SDL_AtomicGet(&mSize);}
    
    void pushFront(const T &item) {insert(begin(), item);}
    void pushBack(const T &item) {insert(end(), item);}
    void insert(const Iterator &itr, const T &item) {insertNode(itr.mNode, item);}
    
    void popFront() {remove(begin());}
    void popBack() {remove(end());}
    void remove(Iterator &itr) {removeNode(&itr.mNode);}
    
    bool remove(const T &item)
    {
      bool success = false;
      Iterator itr = begin();
      while (itr != end())
      {
        if (*itr == item)
        {
          remove(itr);
          success = true;
        }
        else
          ++itr;
      }
      return success;
    }

    void clear()
    {
      Iterator itr(this, mRoot->next);
      while (itr != end())
        remove(itr);
    }
    
    static void CleanUpPool() {NodePool.cleanUp();}
  private:
    friend Iterator;
    
    void assignNode(Node **ref, Node *node)
    {
      *ref = node;
    }
    void incrementNode(Node **ref)
    {
      if (*ref)
        *ref = (*ref)->next;
    }
    void decrementNode(Node **ref)
    {
      if (*ref)
        *ref = (*ref)->prev;
    }
    void insertNode(Node *ptr, const T &item)
    {
      Node *node = NodePool.newItem();
      node->item = item;
      if (ptr == mRoot)
      {
        node->next = mRoot;
        node->prev = mRoot->prev;
        mRoot->prev->next = node;
        mRoot->prev = node;
      }
      else
      {
        node->next = ptr->next;
        node->prev = ptr;
        ptr->next->prev = node;
        ptr->next = node;
      }
      SDL_AtomicAdd(&mSize, 1);
    }
    void removeNode(Node **ref)
    {
      Node *next = nullptr;
      if (*ref && *ref != mRoot)
      {
        next = (*ref)->next;
        next->prev = (*ref)->prev;
        (*ref)->prev->next = next;
        NodePool.freeItem(*ref);
        *ref = next;
        SDL_AtomicAdd(&mSize, -1);
      }
    }
    
    void init()
    {
      SDL_AtomicSet(&mSize, 0);
      mRoot = NodePool.newItem();
      mRoot->next = mRoot->prev = mRoot;
    }
    
    Node *mRoot;
    SDL_atomic_t mSize;
  };
  
  template <typename T>
  Pool<typename List<T>::Node> List<T>::NodePool(DEFAULT_LIST_POOL_SIZE);
}


#endif /* List_h */
