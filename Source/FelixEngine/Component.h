//
//  Component.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/7/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Component_h
#define Component_h

#include <iostream>
#include <map>

#include "EventHandler.h"
#include "XMLTree.h"
#include "Delegate.h"
#include "Mutex.h"


namespace fx
{
  class Scene;
  
  /**
   *
   */
  class Component: public EventHandler
  {
  public:
    Component(const std::string &type, Scene *scene);
    virtual ~Component();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
    
    virtual void update()
    {
      for (iterator itr = begin(); itr != end(); ++itr)
        (*itr)->update();
    }
    
//    typedef Delegate<void, void*> UpdateDelegate;
//    UpdateDelegate& getUpdateDelegate() {return mUpdateDelegate;}
    
    void setName(const std::string &name) {mName = name;}
    std::string name() const {return mName;}
    
    std::string type() const {return mType;}
    Component* parrent() const {return mParrent;}
    
    void lock() const {mMutex.lock();}
    void unlock() const {mMutex.unlock();}
    
  public:
    typedef std::list<Component*>::iterator iterator;
    iterator begin() {return mChildren.begin();}
    iterator end() {return mChildren.end();}
    
    bool addChildren(const XMLTree::Node &node);
    void addChild(Component *child);
    void removeChild(Component *child);
    iterator removeChild(iterator itr);
    
    void deleteChild(Component *child);
    iterator deleteChild(iterator itr);
    
    Component* getChildByName(const std::string &name);
    Component* getChildByType(const std::string &type);

    
    void clearChildren();
    
    /**
     * Internal class used for reflection with xml
     */
    struct ComponentId
    {
      virtual ~ComponentId() {}
      virtual Component* create(Scene *scene) = 0;
    };
    static std::map<std::string, ComponentId*>& GetComponentIdMap();
    
    /**
     *
     */
    static Component* Create(const std::string &type, Scene *scene)
    {
      if (GetComponentIdMap().count(type))
        return GetComponentIdMap().at(type)->create(scene);
      //std::cerr << "Error: Unknown Object Component: " << type << std::endl;
      return nullptr;
    }
    
    /**
     *
     */
    static Component* Create(const XMLTree::Node *node, Scene *scene)
    {
      Component *comp = nullptr;
      if (node)
      {
        comp = Create(node->element(), scene);
        if (!comp || !comp->setToXml(node))
        {
          delete comp;
          comp = nullptr;
        }
      }
      return comp;
    }
  protected:
    std::string mName;
    std::string mType;
    Scene *mScene;
    
    Component *mParrent;
    std::list<Component*> mChildren;
    
    mutable Mutex mMutex;
  };
  
  /**
   * Macro used to define ComponentId classes.
   */
  #define DEFINE_COMPONENT_ID(T) \
    struct T##ID: public fx::Component::ComponentId {\
      T##ID() {fx::Component::GetComponentIdMap()[#T] = this;}\
      virtual ~T##ID() {}\
      virtual fx::Component* create(fx::Scene *scene) {return new T(scene);}\
      static T##ID ID;\
    };\
    T##ID T##ID::ID;
}

#endif /* Component_h */
