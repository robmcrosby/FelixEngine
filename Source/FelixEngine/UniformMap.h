//
//  UniformMap.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/11/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef UniformMap_h
#define UniformMap_h

//#include "Variant.h"
//#include "FelixEngine.h"
//#include "GraphicSystem.h"
//#include "XMLTree.h"
//
//
//namespace fx
//{
//  struct InternalUniformMap
//  {
//    virtual ~InternalUniformMap() {}
//    virtual void release() = 0;
//  };
//  
//  class Uniform: public Variant
//  {
//  public:
//    Uniform(const Variant &var): Variant(var), mInstancedDivisor(0), mBufferId(0) {}
//    Uniform(VAR_TYPE type = VAR_UNKNOWN, size_t size = 1): Variant(type, size), mInstancedDivisor(0), mBufferId(0) {}
//    Uniform(const std::string &type, const std::string &str): Variant(type, str), mInstancedDivisor(0), mBufferId(0) {}
//    
//    Uniform(const XMLTree::Node *node): Variant(node), mInstancedDivisor(0), mBufferId(0) {}
//    Uniform(const XMLTree::Node &node): Variant(node), mInstancedDivisor(0), mBufferId(0) {}
//    
//    Uniform(const float &value): Variant(value), mInstancedDivisor(0), mBufferId(0) {}
//    Uniform(const vec2  &value): Variant(value), mInstancedDivisor(0), mBufferId(0) {}
//    Uniform(const vec3  &value): Variant(value), mInstancedDivisor(0), mBufferId(0) {}
//    Uniform(const vec4  &value): Variant(value), mInstancedDivisor(0), mBufferId(0) {}
//    Uniform(const RGBAf &value): Variant(value), mInstancedDivisor(0), mBufferId(0) {}
//    
//    Uniform(const int   &value): Variant(value), mInstancedDivisor(0), mBufferId(0) {}
//    Uniform(const ivec2 &value): Variant(value), mInstancedDivisor(0), mBufferId(0) {}
//    Uniform(const ivec3 &value): Variant(value), mInstancedDivisor(0), mBufferId(0) {}
//    Uniform(const ivec4 &value): Variant(value), mInstancedDivisor(0), mBufferId(0) {}
//    
//    Uniform(const mat2 &value): Variant(value), mInstancedDivisor(0), mBufferId(0) {}
//    Uniform(const mat3 &value): Variant(value), mInstancedDivisor(0), mBufferId(0) {}
//    Uniform(const mat4 &value): Variant(value), mInstancedDivisor(0), mBufferId(0) {}
//    virtual ~Uniform() {}
//    
//    int instanceDivisor() const {return mInstancedDivisor;}
//    void setInstanceDivisor(int divisor) {mInstancedDivisor = divisor;}
//    
//    int bufferId() const {return mBufferId;}
//    void setBufferId(int buffId) {mBufferId = buffId;}
//    
//  private:
//    int mInstancedDivisor;
//    int mBufferId;
//  };
//  
//  
//  class UniformMap
//  {
//  public:
//    UniformMap(): mInternalMap(0), mLock(0) {setInternalMap();}
//    UniformMap(const UniformMap &that): mInternalMap(0), mLock(0)
//    {
//      setInternalMap();
//      *this = that;
//    }
//    virtual ~UniformMap() {mInternalMap->release();}
//    
//    UniformMap& operator=(const UniformMap &that)
//    {
//      mMap = that.mMap;
//      return *this;
//    }
//    
//    void set(const std::string &key, const Uniform &uniform)
//    {
//      lock();
//      mMap[key] = uniform;
//      unlock();
//    }
//    
//    Uniform& operator[](const std::string &key) {return mMap[key];}
//    void erase(const std::string &key) {mMap.erase(key);}
//    void clear() {mMap.clear();}
//    
//    typedef std::map<std::string, Uniform>::iterator iterator;
//    iterator begin() {return mMap.begin();}
//    iterator end()   {return mMap.end();}
//    
//    typedef std::map<std::string, Uniform>::const_iterator const_iterator;
//    const_iterator begin() const {return mMap.begin();}
//    const_iterator end()   const {return mMap.end();}
//    
//    size_t size() const {return mMap.size();}
//    size_t sizeInBytes() const
//    {
//      size_t totalSize = 0;
//      for (const_iterator itr = begin(); itr != end(); ++itr)
//        totalSize += itr->second.sizeInBytes();
//      return totalSize;
//    }
//    
//    InternalUniformMap* getInternalMap() const {return mInternalMap;}
//    
//    bool setToXml(const XMLTree::Node *node) {return node && setToXml(*node);}
//    bool setToXml(const XMLTree::Node &node)
//    {
//      bool success = true;
//      for (XMLTree::const_iterator itr = node.begin(); itr != node.end(); ++itr)
//      {
//        if (*itr && (*itr)->hasAttribute("name") && (*itr)->hasAttribute("type"))
//          mMap[(*itr)->attribute("name")] = *itr;
//        else
//        {
//          std::cerr << "Error reading Uniform Node: " << std::endl;
//          std::cerr << **itr;
//          success = false;
//        }
//      }
//      return success;
//    }
//    
//    void lock() const {SDL_AtomicLock(&mLock);}
//    void unlock() const {SDL_AtomicUnlock(&mLock);}
//    
//  protected:
//    void setInternalMap()
//    {
//      GraphicSystem *system = FelixEngine::GetGraphicSystem();
//      if (system)
//        mInternalMap = system->getInternalUniformMap(this);
//    }
//    
//  private:
//    std::map<std::string, Uniform> mMap;
//    mutable SDL_SpinLock mLock;
//    InternalUniformMap *mInternalMap;
//  };
//  
//  typedef std::pair<std::string, Uniform> UniformPair;
//  typedef std::list<UniformPair> UniformList;
//}

#endif /* UniformMap_h */
