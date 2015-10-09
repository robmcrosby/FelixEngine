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

#include "XMLTree.h"


namespace fx
{
  /**
   *
   */
  class Component
  {
  public:
    Component(const std::string &type): mType(type) {}
    virtual ~Component() {}
    
    virtual bool setToXml(const XMLTree::Node *node)
    {
      bool success = false;
      if (node)
      {
        success = true;
        if (node->hasAttribute("name"))
          setName(node->attribute("name"));
        
        std::cout << "Created Component: " << type() << " with name: " << name() << std::endl;
      }
      return success;
    }
    
    void setName(const std::string &name) {mName = name;}
    std::string name() const {return mName;}
    
    std::string type() const {return mType;}
    
    /**
     * Internal class used for reflection with xml
     */
    struct ComponentId
    {
      virtual ~ComponentId() {}
      virtual Component* create() = 0;
    };
    static std::map<std::string, ComponentId*>& GetComponentIdMap()
    {
      static std::map<std::string, ComponentId*> compIdMap;
      return compIdMap;
    }
    
    /**
     *
     */
    static Component* Create(const std::string &type)
    {
      if (GetComponentIdMap().count(type))
        return GetComponentIdMap().at(type)->create();
      std::cerr << "Error: Unknown Object Component: " << type << std::endl;
      return nullptr;
    }
    
    /**
     *
     */
    static Component* Create(const XMLTree::Node *node)
    {
      Component *comp = nullptr;
      if (node)
      {
        comp = Create(node->element());
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
  };
  
  /**
   * Macro used to define ComponentId classes.
   */
  #define DEFINE_COMPONENT_ID(T) \
    struct T##ID: public Component::ComponentId {\
      T##ID() {Component::GetComponentIdMap()[#T] = this;}\
      virtual ~T##ID() {}\
      virtual Component* create() {return new T();}\
      static T##ID ID;\
    };\
    T##ID T##ID::ID;
  }

#endif /* Component_h */
