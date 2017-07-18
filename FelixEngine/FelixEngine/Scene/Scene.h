//
//  Scene.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Scene_h
#define Scene_h

#include "RenderPass.h"
#include "XMLTree.h"
#include <set>
#include <map>
#include <string>

namespace fx {
  class Model;
  class Camera;
  class Material;
  class LightRig;
  
  class Scene {
  protected:
    RenderPasses _renderPasses;
    
    std::set<ShaderProgram*> _shaders;
    std::map<std::string, ShaderProgram*> _shaderMap;
    
    std::set<VertexMesh*> _meshes;
    std::map<std::string, VertexMesh*> _mesheMap;
    
    std::map<std::string, Model*>    _models;
    std::map<std::string, Camera*>   _cameras;
    std::map<std::string, Material*> _materials;
    std::map<std::string, LightRig*> _lights;
    
  public:
    bool loadXMLFile(const std::string &file);
    bool loadXML(const XMLTree::Node &node);
    
  private:
    bool addShader(const XMLTree::Node &node);
    bool addMesh(const XMLTree::Node &node);
    bool addLightRig(const XMLTree::Node &node);
    bool addMaterial(const XMLTree::Node &node);
    bool addCamera(const XMLTree::Node &node);
    bool addModel(const XMLTree::Node &node);
    
  public:
    Scene();
    virtual ~Scene();
    
    virtual void update();
    virtual void render();
    
    RenderPasses& renderPasses() {return _renderPasses;}
    
    ShaderProgram* getShader(const std::string &name);
    VertexMesh* getMesh(const std::string &name);
    
    Model* getModel(const std::string &name);
    Model* setModel(Model *model, const std::string &name);
    
    Camera* getCamera(const std::string &name);
    Camera* setCamera(Camera *camera, const std::string &name);
    
    Material* getMaterial(const std::string &name);
    Material* setMaterial(Material *material, const std::string &name);
    
    LightRig* getLightRig(const std::string &name);
    LightRig* setLightRig(LightRig *light, const std::string &name);
  };
  
}

#endif /* Scene_h */
