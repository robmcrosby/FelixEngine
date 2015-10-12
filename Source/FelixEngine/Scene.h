//
//  Scene.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/2/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Scene_h
#define Scene_h

#include <istream>
#include <list>
#include "XMLTree.h"
#include "Object.h"
#include "View.h"
#include "Material.h"

namespace fx
{
  class Scene
  {
  public:
    Scene() {}
    ~Scene() {clearObjects();}
    
    bool setToXml(const XMLTree::Node *node)
    {
      bool success = false;
      if (node && node->hasAttribute("name"))
      {
        success = true;
        setName(node->attribute("name"));
        for (XMLTree::const_iterator itr = node->begin(); itr != node->end(); ++itr)
          success &= addObject(createObject(*itr));
      }
      return success;
    }
    bool init()
    {
      bool success = true;
      for (iterator itr = begin(); itr != end(); ++itr)
        success &= (*itr)->init();
      return success;
    }
    void update()
    {
      for (iterator itr = begin(); itr != end(); ++itr)
        (*itr)->update();
    }
    
    void setName(const std::string &name) {mName = name;}
    std::string name() const {return mName;}
    
    typedef std::list<Object*>::iterator iterator;
    iterator begin() {return mObjects.begin();}
    iterator end() {return mObjects.end();}
    
    typedef std::list<Object*>::const_iterator const_iterator;
    const_iterator begin() const {return mObjects.begin();}
    const_iterator end() const {return mObjects.end();}
    
    bool addObject(Object *obj)
    {
      if (obj)
        mObjects.push_back(obj);
      return obj;
    }
    void clearObjects()
    {
      for (iterator itr = begin(); itr != end(); ++itr)
        delete *itr;
      mObjects.clear();
    }
    
    Object* getObjectByName(const std::string &name)
    {
      for (iterator itr = begin(); itr != end(); ++itr)
      {
        if ((*itr)->name() == name)
          return *itr;
      }
      return nullptr;
    }
    Object* getObjectByType(const std::string &type)
    {
      for (iterator itr = begin(); itr != end(); ++itr)
      {
        if ((*itr)->type() == type)
          return *itr;
      }
      return nullptr;
    }
    
  private:
    Object* createObject(const XMLTree::Node *node)
    {
      Object *obj = new Object(this);
      if (!node || !obj->setToXml(node))
      {
        delete obj;
        obj = nullptr;
      }
      return obj;
    }
    
    std::string mName;
    std::list<Object*> mObjects;
  };
}

#endif /* Scene_h */
