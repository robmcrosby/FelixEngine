//
//  IndexedMap.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/24/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef IndexedMap_h
#define IndexedMap_h

#include <map>
#include <vector>

namespace fx
{
  template <typename Key, typename Type>
  class IndexedMap
  {
  private:
    std::vector<Type> _array;
    std::map<Key, int> _map;
    
  public:
    IndexedMap() {}
    IndexedMap(const IndexedMap &other) {*this = other;}
    virtual ~IndexedMap() {}
    
    IndexedMap& operator=(const IndexedMap &other)
    {
      _map = other._map;
      _array = other._array;
      return *this;
    }
    
    typedef typename std::vector<Type>::iterator iterator;
    iterator begin() {return _array.begin();}
    iterator end()   {return _array.end();}
    
    typedef typename std::vector<Type>::const_iterator const_iterator;
    const_iterator begin() const {return _array.begin();}
    const_iterator end()   const {return _array.end();}
    
    
    class map_iterator
    {
    private:
      IndexedMap<Key, Type> *_indexedMap;
      typename std::map<Key, int>::iterator _mapIterator;
      
    public:
      map_iterator(): _indexedMap(0) {}
      map_iterator(IndexedMap<Key, Type> *map, typename std::map<Key, int>::iterator itr):
      _indexedMap(map), _mapIterator(itr) {}
      
      map_iterator& operator++()
      {
        if (_indexedMap)
          ++_mapIterator;
      }
      bool operator==(const map_iterator &other) const
      {
        return !_indexedMap || _mapIterator == other._mapIterator;
      }
      Key first() const {return _indexedMap ? _mapIterator->first : Key();}
      Type& second() {return (*_indexedMap)[_mapIterator->second];}
      const Type& second() const {return (*_indexedMap)[_mapIterator->second];}
    };
    map_iterator mapBegin() {return MapIterator(this, _map.begin());}
    map_iterator mapEnd() {return MapIterator(this, _map.end());}
    
    
    class const_map_iterator
    {
    private:
      const IndexedMap<Key, Type> *_indexedMap;
      typename std::map<Key, int>::const_iterator _mapIterator;
      
    public:
      const_map_iterator(): _indexedMap(0) {}
      const_map_iterator(const IndexedMap<Key, Type> *map, typename std::map<Key, int>::iterator itr):
      _indexedMap(map), _mapIterator(itr) {}
      
      map_iterator& operator++()
      {
        if (_indexedMap)
          ++_mapIterator;
      }
      bool operator==(const map_iterator &other) const
      {
        return !_indexedMap || _mapIterator == other._mapIterator;
      }
      Key first() const {return _indexedMap ? _mapIterator->first : Key();}
      const Type& second() const {return (*_indexedMap)[_mapIterator->second];}
    };
    const_map_iterator mapBegin() const {return MapIterator(this, _map.begin());}
    const_map_iterator mapEnd() const {return MapIterator(this, _map.end());}
    
    size_t size() const {return _array.size();}
    bool contains(const Key &key) const {return _map.count(key);}
    
    Type& operator[](int index) {return _array[index];}
    Type& operator[](const Key &key)
    {
      int index = 0;
      typename std::map<Key, int>::iterator itr = _map.find(key);
      if (itr != _map.end())
        index = itr->second;
      else
        index = push(key);
      return _array[index];
    }
    
    Type* at(int index) {return index < (int)_array.size() ? &_array.at(index) : NULL;}
    Type* at(const Key &key)
    {
      typename std::map<Key, int>::iterator itr = _map.find(key);
      return itr != _map.end() ? at(itr->second) : NULL;
    }
    
    const Type* at(int index) const {return index < (int)_array.size() ? &_array.at(index) : NULL;}
    const Type* at(const Key &key) const
    {
      typename std::map<Key, int>::const_iterator itr = _map.find(key);
      return itr != _map.end() ? at(itr->second) : NULL;
    }
    
    int getIndex(const Key &key) const {return _map.count(key) ? _map.at(key) : -1;}
    
    int push(const Key &key = "", const Type &item = Type())
    {
      int index = (int)_array.size();
      if (key != "")
        _map[key] = index;
      _array.push_back(item);
      return index;
    }
    
    Type& back() {return _array.back();}
    Type& front() {return _array.front();}
    
    void clear()
    {
      _map.clear();
      _array.clear();
    }
  };
}

#endif /* IndexedMap_h */
