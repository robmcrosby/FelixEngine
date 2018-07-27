//
//  iObject.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/24/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef iObject_h
#define iObject_h

#include "XMLTree.h"
#include <set>


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
}

#endif /* iObject_h */
