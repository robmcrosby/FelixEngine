//
//  IndexedMap.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/11/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef IndexedMap_h
#define IndexedMap_h

#include <map>
#include <vector>
#include "Semaphore.h"

namespace fx
{
  template <typename T>
  class IndexedMap
  {
  public:
    IndexedMap() {}
    IndexedMap(const IndexedMap &other) {*this = other;}
    virtual ~IndexedMap() {}
    
    IndexedMap& operator=(const IndexedMap &other)
    {
      mSemephore.lock();
      other.wait();
      
      mMap = other.mMap;
      mArray = other.mArray;
      
      other.post();
      mSemephore.unlock();
      
      return *this;
    }
    
    typedef typename std::vector<T>::iterator iterator;
    iterator begin() {return mArray.begin();}
    iterator end()   {return mArray.end();}
    
    typedef typename std::vector<T>::const_iterator const_iterator;
    const_iterator begin() const {return mArray.begin();}
    const_iterator end()   const {return mArray.end();}
    
    size_t size() const {return mArray.size();}
    
    T& operator[](int index) {return mArray[index];}
    T& operator[](const std::string &key)
    {
      int index = 0;
      std::map<std::string, int>::iterator itr = mMap.find(key);
      if (itr != mMap.end())
        index = itr->second;
      else
      {
        mSemephore.post();
        index = push(key);
        mSemephore.wait();
      }
      return mArray[index];
    }
    
    T* at(int index) {return index < (int)mArray.size() ? &mArray.at(index) : NULL;}
    T* at(const std::string &key)
    {
      std::map<std::string, int>::iterator itr = mMap.find(key);
      return itr != mMap.end() ? at(itr->second) : NULL;
    }
    
    const T* at(int index) const {return index < (int)mArray.size() ? &mArray.at(index) : NULL;}
    const T* at(const std::string &key) const
    {
      std::map<std::string, int>::const_iterator itr = mMap.find(key);
      return itr != mMap.end() ? at(itr->second) : NULL;
    }
    
    int getIndex(const std::string &key) const {return mMap.count(key) ? mMap.at(key) : -1;}
    
    int push(const std::string &key = "", const T &item = T())
    {
      mSemephore.lock();
      int index = (int)mArray.size();
      if (key != "")
        mMap[key] = index;
      mArray.push_back(item);
      mSemephore.unlock();
      
      return index;
    }
    
    T& back() {return mArray.back();}
    T& front() {return mArray.front();}
    
    void clear()
    {
      mSemephore.lock();
      mMap.clear();
      mArray.clear();
      mSemephore.unlock();
    }
    
    void lock()   const {mSemephore.lock();}
    void unlock() const {mSemephore.unlock();}
    
    void wait() const {mSemephore.wait();}
    void post() const {mSemephore.post();}
    
  private:
    std::vector<T> mArray;
    std::map<std::string, int> mMap;
    mutable Semaphore mSemephore;
  };
}

#endif /* IndexedMap_h */
