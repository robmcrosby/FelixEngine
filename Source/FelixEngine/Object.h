//
//  Object.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/7/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Object_h
#define Object_h

#include <list>
#include "Component.h"


namespace fx
{
  class Scene;
  /**
   *
   */
  class Object
  {
  public:
    Object(Scene *scene): mScene(scene) {}
    ~Object() {clearComponents();}
    
    bool setToXml(const XMLTree::Node *node)
    {
      bool success = false;
      if (node)
      {
        success = true;
        setType(node->element());
        if (node->hasAttribute("name"))
          setName(node->attribute("name"));
        
        for (XMLTree::const_iterator itr = node->begin(); itr != node->end(); ++itr)
          success &= addComponent(Component::Create(*itr, this));
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
        (*itr)->getUpdateDelegate()(nullptr);
    }
    
    void setName(const std::string &name) {mName = name;}
    std::string name() const {return mName;}
    
    void setType(const std::string &type) {mType = type;}
    std::string type() const {return mType;}
    
    Scene* getScene() const {return mScene;}
    
    typedef std::list<Component*>::iterator iterator;
    iterator begin() {return mComponents.begin();}
    iterator end() {return mComponents.end();}
    
    typedef std::list<Component*>::const_iterator const_iterator;
    const_iterator begin() const {return mComponents.begin();}
    const_iterator end() const {return mComponents.end();}
    
    bool addComponent(Component *comp)
    {
      if (comp)
        mComponents.push_back(comp);
      return comp;
    }
    void clearComponents()
    {
      for (iterator itr = begin(); itr != end(); ++itr)
        delete *itr;
      mComponents.clear();
    }
    
    Component* getComponentByName(const std::string &name)
    {
      for (iterator itr = begin(); itr != end(); ++itr)
      {
        if ((*itr)->name() == name)
          return *itr;
      }
      return nullptr;
    }
    Component* getComponentByType(const std::string &type)
    {
      for (iterator itr = begin(); itr != end(); ++itr)
      {
        if ((*itr)->type() == type)
          return *itr;
      }
      return nullptr;
    }
    
  private:
    std::string mName, mType;
    std::list<Component*> mComponents;
    Scene *mScene;
  };
}

#endif /* Object_hpp */
