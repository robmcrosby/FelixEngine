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
#include "GraphicSystem.h"
#include "XMLTree.h"


namespace fx
{
  class UniformMap;
  
  typedef Variant Uniform;
  
  struct InternalUniformMap
  {
    virtual ~InternalUniformMap() {}
    virtual void update(const VariantMap &map) = 0;
  };
  
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
      mNextMap = that.mNextMap;
      mCurrMap = that.mCurrMap;
      if (mInternalMap)
        mInternalMap->update(mCurrMap);
      return *this;
    }
    
    Variant& operator[](const std::string &key) {return mNextMap[key];}
    void erase(const std::string &key) {mNextMap.erase(key);}
    void clear() {mNextMap.clear();}
    
    iterator begin() {return mNextMap.begin();}
    iterator end()   {return mNextMap.end();}
    
    const_iterator begin() const {return mCurrMap.begin();}
    const_iterator end()   const {return mCurrMap.end();}
    
    void sync()
    {
      mCurrMap = mNextMap;
      if (mInternalMap)
        mInternalMap->update(mCurrMap);
    }
    
    InternalUniformMap* getInternalMap() const {return mInternalMap;}
    
    bool setToXml(const XMLTree::Node &node)
    {
      Uniform uniform;
      
      if (node != "Uniforms")
      {
        std::cerr << "Invalid Uniforms Node" << std::endl;
        return false;
      }
      
      for (XMLTree::const_iterator itr = node.begin(); itr != node.end(); ++itr)
      {
        const XMLTree::Node &subNode = **itr;
        std::string name = subNode.attribute("name");
        if (subNode == "Uniform" && name != "")
        {
          uniform.parse(subNode.attribute("type"), subNode.contents());
          if (uniform.type() != VAR_UNKNOWN)
            mNextMap[name] = uniform;
        }
      }
      
      sync();
      return true;
    }
    
  protected:
    void setInternalMap()
    {
      //GraphicSystem *system = FelixEngine::GetGraphicSystem();
      //if (system)
      //  mInternalMap = system->createUniformMap();
    }
    
    VariantMap mNextMap;
    VariantMap mCurrMap;
    
    InternalUniformMap *mInternalMap;
  };
}

#endif /* UniformMap_h */
