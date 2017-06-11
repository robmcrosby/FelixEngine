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
    
    class map_iterator
    {
    public:
      map_iterator(): mIndexedMap(0) {}
      map_iterator(IndexedMap<T> *map, std::map<std::string, int>::iterator itr):
      mIndexedMap(map), mMapIterator(itr) {}
      
      map_iterator& operator++()
      {
        if (mIndexedMap)
          ++mMapIterator;
      }
      bool operator==(const map_iterator &other) const
      {
        return !mIndexedMap || mMapIterator == other.mMapIterator;
      }
      std::string first() const {return mIndexedMap ? mMapIterator->first : "";}
      T& second() {return (*mIndexedMap)[mMapIterator->second];}
      const T& second() const {return (*mIndexedMap)[mMapIterator->second];}
      
    private:
      IndexedMap<T> *mIndexedMap;
      std::map<std::string, int>::iterator mMapIterator;
    };
    map_iterator mapBegin() {return MapIterator(this, mMap.begin());}
    map_iterator mapEnd() {return MapIterator(this, mMap.end());}
    
    class const_map_iterator
    {
    public:
      const_map_iterator(): mIndexedMap(0) {}
      const_map_iterator(const IndexedMap<T> *map, std::map<std::string, int>::iterator itr):
      mIndexedMap(map), mMapIterator(itr) {}
      
      map_iterator& operator++()
      {
        if (mIndexedMap)
          ++mMapIterator;
      }
      bool operator==(const map_iterator &other) const
      {
        return !mIndexedMap || mMapIterator == other.mMapIterator;
      }
      std::string first() const {return mIndexedMap ? mMapIterator->first : "";}
      const T& second() const {return (*mIndexedMap)[mMapIterator->second];}
      
    private:
      const IndexedMap<T> *mIndexedMap;
      std::map<std::string, int>::const_iterator mMapIterator;
    };
    const_map_iterator mapBegin() const {return MapIterator(this, mMap.begin());}
    const_map_iterator mapEnd() const {return MapIterator(this, mMap.end());}
    
    size_t size() const {return mArray.size();}
    bool contains(const std::string &name) const {return mMap.count(name);}
    
    T& operator[](int index) {return mArray[index];}
    T& operator[](const std::string &key)
    {
      int index = 0;
      std::map<std::string, int>::iterator itr = mMap.find(key);
      if (itr != mMap.end())
        index = itr->second;
      else
        index = push(key);
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
    mutable IndexedSemaphore mSemephore;
  };
}

#endif /* IndexedMap_h */
