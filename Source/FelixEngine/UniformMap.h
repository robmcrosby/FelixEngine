//
//  UniformMap.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/11/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef UniformMap_h
#define UniformMap_h

#include "Variant.h"
#include "FelixEngine.h"
#include "GraphicSystem.h"
#include "XMLTree.h"


namespace fx
{
  struct InternalUniformMap
  {
    virtual ~InternalUniformMap() {}
    virtual void release() = 0;
  };
  
  typedef Variant Uniform;
  class UniformMap
  {
  public:
    typedef VariantMap::const_iterator iterator;
    
  public:
    UniformMap(): mInternalMap(0), mLock(0) {setInternalMap();}
    UniformMap(const UniformMap &that): mInternalMap(0), mLock(0)
    {
      setInternalMap();
      *this = that;
    }
    virtual ~UniformMap() {mInternalMap->release();}
    
    UniformMap& operator=(const UniformMap &that)
    {
      mMap = that.mMap;
      return *this;
    }
    
    void set(const std::string &key, const Uniform &uniform)
    {
      lock();
      if (key == "View" && uniform.mat4Value() == mat4())
        std::cout << "Idenity" << std::endl;
      mMap[key] = uniform;
      unlock();
    }
    
    Variant& operator[](const std::string &key) {return mMap[key];}
    void erase(const std::string &key) {mMap.erase(key);}
    void clear() {mMap.clear();}
    
    iterator begin() const {return mMap.begin();}
    iterator end()   const {return mMap.end();}
    
    size_t size() const {return mMap.size();}
    size_t sizeInBytes() const
    {
      size_t totalSize = 0;
      for (iterator itr = begin(); itr != end(); ++itr)
        totalSize += itr->second.sizeInBytes();
      return totalSize;
    }
    
    InternalUniformMap* getInternalMap() const {return mInternalMap;}
    const VariantMap& getVariantMap() const {return mMap;}
    
    bool setToXml(const XMLTree::Node *node) {return node && setToXml(*node);}
    bool setToXml(const XMLTree::Node &node)
    {
      bool success = true;
      for (XMLTree::const_iterator itr = node.begin(); itr != node.end(); ++itr)
      {
        if (*itr && (*itr)->hasAttribute("name") && (*itr)->hasAttribute("type"))
          mMap[(*itr)->attribute("name")] = *itr;
        else
        {
          std::cerr << "Error reading Uniform Node: " << std::endl;
          std::cerr << **itr;
          success = false;
        }
      }
      return success;
    }
    
    void lock() const {SDL_AtomicLock(&mLock);}
    void unlock() const {SDL_AtomicUnlock(&mLock);}
    
  protected:
    void setInternalMap()
    {
      GraphicSystem *system = FelixEngine::GetGraphicSystem();
      if (system)
        mInternalMap = system->getInternalUniformMap(this);
    }
    
  private:
    VariantMap mMap;
    mutable SDL_SpinLock mLock;
    InternalUniformMap *mInternalMap;
  };
}

#endif /* UniformMap_h */
