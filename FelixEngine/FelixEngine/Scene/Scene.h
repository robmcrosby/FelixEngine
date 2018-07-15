//
//  Scene.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Scene_h
#define Scene_h

#include "EventSubject.h"
#include "XMLTree.h"
#include <memory>
#include <string>
#include <set>
#include <map>


namespace fx {
  class Scene;
  typedef std::shared_ptr<Scene> ScenePtr;
  
  struct iObject {
    virtual ~iObject() {}
    virtual void setScene(Scene *scene) = 0;
    virtual bool loadXML(const XMLTree::Node &node) = 0;
    virtual void update(float dt) = 0;
  };
  typedef std::shared_ptr<iObject> SharedObject;
  typedef std::weak_ptr<iObject> WeakObject;
  
  typedef std::set<SharedObject> Objects;
  typedef std::map<std::string, WeakObject> ObjectMap;
  
  struct iObjectBuilder {
    virtual SharedObject build(Scene *scene, const std::string &name) = 0;
  };
  typedef std::map<std::string, iObjectBuilder*> BuilderMap;
  
  #define DEFINE_OBJ_BUILDER(T) \
    class T; \
    typedef std::shared_ptr<T> T##Ptr; \
    struct T##Builder: public iObjectBuilder { \
      T##Builder() {Scene::getBuilderMap()[#T] = this;} \
      virtual ~T##Builder() {} \
      virtual SharedObject build(Scene *scene, const std::string &name) { \
        T##Ptr obj = scene->get<T>(name); \
        return std::static_pointer_cast<iObject>(obj); \
      } \
    };
  
//  class Model;
//  typedef std::shared_ptr<Model> ModelPtr;
//
//  struct ModelBuilder: public iObjectBuilder {
//    ModelBuilder() {Scene::getBuilderMap()["Model"] = this;}
//    virtual ~ModelBuilder() {}
//    virtual SharedObject build(Scene *scene, const std::string &name) {
//      ModelPtr obj = scene->get<Model>(name);
//      return std::static_pointer_cast<iObject>(obj);
//    }
//  };
  
  
  
  class Scene: public EventSubject {
  private:
    Objects   _objects;
    ObjectMap _objectMap;
    
  public:
    Scene();
    ~Scene();
    
    bool loadXMLFile(const std::string &file);
    bool loadXML(const XMLTree::Node &node);
    bool loadObject(const XMLTree::Node &node);
    
    template <typename T, typename... Args>
    std::shared_ptr<T> make(const std::string &name, Args... args) {
      std::shared_ptr<T> item = std::make_shared<T>(args...);
      SharedObject obj = std::static_pointer_cast<iObject>(item);
      addObject(obj, name);
      return item;
    }
    
    template <typename T>
    std::shared_ptr<T> get(const std::string &name) {
      if (name != "") {
        SharedObject obj = _objectMap[name].lock();
        if (obj) {
          std::shared_ptr<T> item = std::dynamic_pointer_cast<T>(obj);
          if (item)
            return item;
        }
      }
      return make<T>(name);
    }
    
    SharedObject build(const std::string &type, const std::string &name);
    SharedObject build(const XMLTree::Node &node);
    
    void addObject(SharedObject &obj, const std::string &name = "");
    void removeObject(SharedObject &obj);
    void removeObject(const std::string &name);
    
    void update(float td);
    
  public:
    static ScenePtr make();
    static BuilderMap& getBuilderMap();
  };
}

#endif /* Scene_h */
