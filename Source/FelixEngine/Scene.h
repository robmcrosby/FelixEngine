//
//  Scene.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/2/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Scene_h
#define Scene_h


#include "Component.h"
#include "XMLTree.h"
#include "Material.h"
#include "BufferMap.h"

#include <istream>
#include <list>

namespace fx
{
  class Scene: public Component
  {
  public:
    Scene(): Component(this), mDirectory(FileSystem::GetResources())
    {
      mScene = this;
      setEventFlags(EVENT_APP_UPDATE);
    }
    ~Scene()
    {
      clearMaterials();
      clearBufferMaps();
    }
    
    virtual void handle(const Event &event)
    {
      if (event == EVENT_APP_UPDATE)
        update();
    }
    
    virtual void setToXml(const XMLTree::Node &node)
    {
      Component::setToXml(node);
      for (XMLTree::const_iterator itr = node.begin(); itr != node.end(); ++itr)
      {
        if (node.tree() && node.tree()->url() != "")
          mDirectory = node.tree()->url();
        
        if (**itr == "Resources")
          addResources(**itr);
        else
          addChild(**itr);
      }
    }
    bool init()
    {
      bool success = true;
      for (iterator itr = begin(); itr != end(); ++itr)
        success &= (*itr)->init();
      return success;
    }
    
    Material* getMaterial(const std::string &name)
    {
      if (!mMaterialMap.count(name))
        mMaterialMap[name] = new Material();
      return mMaterialMap.at(name);
    }
    void clearMaterials()
    {
      for (std::map<std::string, Material*>::iterator itr = mMaterialMap.begin(); itr != mMaterialMap.end(); ++itr)
        delete itr->second;
      mMaterialMap.clear();
    }
    
    BufferMap& getBufferMap(const std::string name)
    {
      if (!mBufferMaps.count(name))
        mBufferMaps[name] = new BufferMap(name);
      return *mBufferMaps[name];
    }
    void clearBufferMaps()
    {
      for (BufferMapDirectory::iterator itr = mBufferMaps.begin(); itr != mBufferMaps.end(); ++itr)
        delete itr->second;
      mBufferMaps.clear();
    }
    
    int getPassIndex(const std::string &name)
    {
      if (!mPassNameMap.count(name))
        mPassNameMap[name] = (int)mPassNameMap.size()+1;
      return mPassNameMap[name];
    }
    
    File directory() const {return mDirectory;}
    
  private:
    bool addResources(const XMLTree::Node &node)
    {
      bool success = true;
      for (XMLTree::const_iterator itr = node.begin(); itr != node.end(); ++itr)
        success &= addResource(**itr);
      return success;
    }
    bool addResource(const XMLTree::Node &node)
    {
      bool success = true;
      if (node.hasAttribute("name"))
      {
        BufferMap &bufferMap = getBufferMap(node.attribute("name"));
        bufferMap.setToXml(node);
        FelixEngine::GetGraphicSystem()->uploadBuffer(bufferMap);
      }
      return success;
    }
    
    File mDirectory;
    
    std::list<Resource*> mResources;
    std::map<std::string, Material*> mMaterialMap;
    
    std::map<std::string, int> mPassNameMap;
    
    BufferMapDirectory mBufferMaps;
  };
}

#endif /* Scene_h */
