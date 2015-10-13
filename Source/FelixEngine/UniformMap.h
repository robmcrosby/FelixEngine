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
    virtual void update(const VariantMap &map) = 0;
  };
  
  typedef Variant Uniform;
  class UniformMap
  {
  public:
    typedef VariantMap::iterator iterator;
    typedef VariantMap::const_iterator const_iterator;
    
  public:
    UniformMap(): mInternalMap(0) {setInternalMap();}
    UniformMap(const UniformMap &that): mInternalMap(0)
    {
      setInternalMap();
      *this = that;
    }
    virtual ~UniformMap() {delete mInternalMap;}
    
    UniformMap& operator=(const UniformMap &that)
    {
      mMap = that.mMap;
      return *this;
    }
    
    Variant& operator[](const std::string &key) {return mMap[key];}
    void erase(const std::string &key) {mMap.erase(key);}
    void clear() {mMap.clear();}
    
    iterator begin() {return mMap.begin();}
    iterator end()   {return mMap.end();}
    
    const_iterator begin() const {return mMap.begin();}
    const_iterator end()   const {return mMap.end();}
    
    void update() const
    {
      if (mInternalMap)
        mInternalMap->update(mMap);
    }
    
    InternalUniformMap* getInternalMap() const {return mInternalMap;}
    
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
      update();
      return success;
    }
    
  protected:
    void setInternalMap()
    {
      GraphicSystem *system = FelixEngine::GetGraphicSystem();
      if (system)
        mInternalMap = system->createUniformMap();
    }
    
    VariantMap mMap;
    InternalUniformMap *mInternalMap;
  };
}

#endif /* UniformMap_h */
