//
//  Scene.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Scene_h
#define Scene_h

#include "EventHandler.h"
#include "RenderPass.h"
#include "XMLTree.h"
#include "IndexedMap.h"
#include "LightRig.h"
#include <set>
#include <map>
#include <string>

namespace fx {
  class Model;
  class Camera;
  class Material;
  
  class Scene: public EventHandler {
  protected:
    RenderPasses _renderPasses;
    
    IndexedMap<std::string, FramePtr>  _frames;
    IndexedMap<std::string, ShaderPtr> _shaders;
    IndexedMap<std::string, VertexPtr> _meshes;
    
    IndexedMap<std::string, Model*>    _models;
    IndexedMap<std::string, Camera*>   _cameras;
    IndexedMap<std::string, Material*> _materials;
    IndexedMap<std::string, LightRigPtr> _lights;
    
  public:
    bool loadXMLFile(const std::string &file);
    bool loadXML(const XMLTree::Node &node);
    
  private:
    bool addWindow(const XMLTree::Node &node);
    bool addFrame(const XMLTree::Node &node);
    bool addShader(const XMLTree::Node &node);
    bool addMesh(const XMLTree::Node &node);
    bool addLightRig(const XMLTree::Node &node);
    bool addMaterial(const XMLTree::Node &node);
    bool addCamera(const XMLTree::Node &node);
    bool addModel(const XMLTree::Node &node);
    
    Model* createModel(const std::string &name);
    Camera* createCamera(const std::string &name);
    
  public:
    Scene();
    virtual ~Scene();
    
    virtual void update();
    virtual void render();
    
    RenderPasses& renderPasses() {return _renderPasses;}
    
    FramePtr  getFrame(const std::string &name);
    ShaderPtr getShader(const std::string &name);
    VertexPtr getMesh(const std::string &name);
    
    Model*    getModel(const std::string &name = "");
    Camera*   getCamera(const std::string &name = "");
    Material* getMaterial(const std::string &name = "");
    LightRigPtr getLightRig(const std::string &name = "");
  };
  
}

#endif /* Scene_h */
