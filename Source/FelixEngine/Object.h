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
#include "EventHandler.h"
#include "Component.h"


namespace fx
{
  class Scene;
  /**
   *
   */
  class Object: public EventHandler
  {
  public:
    Object(const std::string &type, Scene *scene);
    virtual ~Object();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
    void update()
    {
      for (iterator itr = begin(); itr != end(); ++itr)
        (*itr)->getUpdateDelegate()(nullptr);
    }
    
    void setName(const std::string &name) {mName = name;}
    std::string name() const {return mName;}
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
    void removeComponent(Component *comp)
    {
      mComponents.remove(comp);
    }
    void clearComponents()
    {
      std::list<Component*> tmp = mComponents;
      mComponents.clear();
      for (iterator itr = tmp.begin(); itr != tmp.end(); ++itr)
        delete *itr;
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
    
  public:
    /**
     * Internal class used for reflection with xml
     */
    struct ObjectId
    {
      virtual ~ObjectId() {}
      virtual Object* create(Scene *scene) = 0;
    };
    static std::map<std::string, ObjectId*>& GetObjectIdMap();
    
    /**
     *
     */
    static Object* Create(const std::string &type, Scene *scene)
    {
      if (GetObjectIdMap().count(type))
        return GetObjectIdMap().at(type)->create(scene);
      return new Object(type, scene);
    }
    
    /**
     *
     */
    static Object* Create(const XMLTree::Node *node, Scene *scene)
    {
      Object *obj = nullptr;
      if (node)
      {
        obj = Create(node->element(), scene);
        if (!obj || !obj->setToXml(node))
        {
          delete obj;
          obj = nullptr;
        }
      }
      return obj;
    }
    
  private:
    std::string mName, mType;
    std::list<Component*> mComponents;
    Scene *mScene;
  };
  
  /**
   * Macro used to define ObjectId classes.
   */
  #define DEFINE_OBJECT_ID(T) \
    struct T##ID: public fx::Object::ObjectId {\
    T##ID() {fx::Object::GetObjectIdMap()[#T] = this;}\
    virtual ~T##ID() {}\
    virtual fx::Object* create(fx::Scene *scene) {return new T(scene);}\
    static T##ID ID;\
    };\
    T##ID T##ID::ID;
}

#endif /* Object_hpp */
