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

#include <istream>
#include <list>

namespace fx
{
  class Scene: public Component
  {
  public:
    Scene(): Component(this)
    {
      mScene = this;
      setEventFlags(EVENT_APP_UPDATE);
    }
    ~Scene()
    {
      clearMaterials();
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
      std::string name = node.attribute("name");
      std::string type = node.element();
      if (type == "Material")
      {
        Material *material = getMaterial(name);
        success &= material->setToXml(node);
      }
      else
      {
        Resource *resource = FelixEngine::GetGraphicSystem()->getResource(type, name);
        if (resource)
        {
          resource->retain();
          success &= resource->setToXml(node);
          mResources.push_back(resource);
        }
      }
      return success;
    }
    
    std::list<Resource*> mResources;
    std::map<std::string, Material*> mMaterialMap;
  };
}

#endif /* Scene_h */
