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
#include <SDL2/SDL.h>

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
      Iterator& operator--()
      {
        if (mNode)
          setNode(mNode->prev);
        return *this;
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
    List(): mSize(0) {init();}
    List(const List &other) {*this = other;}
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
    
    bool empty() const {return mRoot == mRoot->next;}
    int size() const {return mSize;}
    
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
    }
    
    void popFront() {remove(begin());}
    void popBack() {remove(end());}
    void clear()
    {
      while (!empty())
        remove(begin());
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
        }
      }
      return next;
    }
    
  private:
    void init()
    {
      mRoot = NodePool.newItem();
      mRoot->next = mRoot->prev = mRoot;
    }
    
    Node *mRoot;
    int   mSize;
  };
  
  template <typename T>
  Pool<typename List<T>::Node> List<T>::NodePool;
}


#endif /* List_h */
