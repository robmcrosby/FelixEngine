//
//  Scene.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Scene_h
#define Scene_h

#include <FelixEngine/EventSubject.h>
#include <FelixEngine/RenderPass.h>
#include <FelixEngine/iObject.h>


namespace fx {
  class USDCrate;
  
  class Scene: public EventSubject {
  private:
    Objects   _objects;
    ObjectMap _objectMap;
    
    std::vector<RenderPassPtr> _renderingPasses;
    std::map<std::string, RenderPassPtr> _renderingPassMap;
    
  public:
    Scene();
    ~Scene();
    
    bool loadXMLFile(const std::string &file, const std::string &pass = "");
    bool loadXML(const XMLTree::Node &node);
    bool loadObject(const XMLTree::Node &node);
    bool loadExternal(const XMLTree::Node &node);
    
    bool loadUSDZFile(const std::string &file, const std::string &pass = "");
    bool loadUSDCrate(const USDCrate &crate, const std::string &pass = "");
    
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
    
    void addRenderingPass(RenderPassPtr renderPass);
    void clearRenderingPasses();
    
    RenderPassPtr createRenderingPass();
    RenderPassPtr getRenderingPass(const std::string &name);
    RenderPassPtr getRenderingPass(const XMLTree::Node &node);
    
    ShaderProgramPtr getShaderProgram(const std::string &name);
    FrameBufferPtr   getFrameBuffer(const std::string &name);
    VertexMeshPtr    getVertexMesh(const std::string &name);
    TextureBufferPtr getTextureBuffer(const std::string &name);
    
  public:
    static ScenePtr make();
    static BuilderMap& getBuilderMap();
  };
}

#endif /* Scene_h */
