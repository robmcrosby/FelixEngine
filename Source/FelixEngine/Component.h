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


namespace fx
{
  class Object;
  
  /**
   *
   */
  class Component: public EventHandler
  {
  public:
    Component(const std::string &type, Object *obj);
    virtual ~Component();
    
    virtual bool setToXml(const XMLTree::Node *node)
    {
      bool success = false;
      if (node)
      {
        success = true;
        if (node->hasAttribute("name"))
          setName(node->attribute("name"));
      }
      return success;
    }
    virtual bool init() {return true;}
    
    void update(void*) {}
    
    typedef Delegate<void, void*> UpdateDelegate;
    UpdateDelegate& getUpdateDelegate() {return mUpdateDelegate;}
    
    void setName(const std::string &name) {mName = name;}
    std::string name() const {return mName;}
    
    std::string type() const {return mType;}
    Object* getObject() const {return mObject;}
    
    /**
     * Internal class used for reflection with xml
     */
    struct ComponentId
    {
      virtual ~ComponentId() {}
      virtual Component* create(Object *obj) = 0;
    };
    static std::map<std::string, ComponentId*>& GetComponentIdMap();
    
    /**
     *
     */
    static Component* Create(const std::string &type, Object *obj)
    {
      if (GetComponentIdMap().count(type))
        return GetComponentIdMap().at(type)->create(obj);
      std::cerr << "Error: Unknown Object Component: " << type << std::endl;
      return nullptr;
    }
    
    /**
     *
     */
    static Component* Create(const XMLTree::Node *node, Object *obj)
    {
      Component *comp = nullptr;
      if (node)
      {
        comp = Create(node->element(), obj);
        if (!comp || !comp->setToXml(node))
        {
          delete comp;
          comp = nullptr;
        }
      }
      return comp;
    }
  protected:
    std::string mName, mType;
    Object *mObject;
    UpdateDelegate mUpdateDelegate;
  };
  
  /**
   * Macro used to define ComponentId classes.
   */
  #define DEFINE_COMPONENT_ID(T) \
    struct T##ID: public fx::Component::ComponentId {\
      T##ID() {fx::Component::GetComponentIdMap()[#T] = this;}\
      virtual ~T##ID() {}\
      virtual fx::Component* create(fx::Object *obj) {return new T(obj);}\
      static T##ID ID;\
    };\
    T##ID T##ID::ID;
}

#endif /* Component_h */
