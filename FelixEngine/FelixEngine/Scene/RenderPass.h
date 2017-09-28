//
//  RenderPass.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/8/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef RenderPass_h
#define RenderPass_h

#include "RenderItem.h"
#include "GraphicTask.h"
#include "UniformMap.h"
#include "Camera.h"
#include "Model.h"
#include "Light.h"


namespace fx {
  typedef std::map<std::string, RenderPassPtr> RenderPassMap;
  typedef std::list<std::string> RenderScheme;
  
  struct ModelCompare {
    bool operator()(Model *lhs, Model *rhs) {
      if (lhs->layer() != rhs->layer())
        return lhs->layer() < rhs->layer();
      return lhs < rhs;
    }
  };
  typedef std::set<Model*, ModelCompare> ModelSet;
  
  struct LightCompare {
    bool operator()(Light *lhs, Light *rhs) {
      if (lhs->layer() != rhs->layer())
        return lhs->layer() < rhs->layer();
      return lhs < rhs;
    }
  };
  typedef std::set<Light*, LightCompare> LightSet;
  
  class RenderPass {
  private:
    Camera *_camera;
    LightSet _lights;
    ModelSet _models;
    
    UniformsPtr _lightUniforms;
    
  public:
    RenderPass();
    ~RenderPass();
    
    void setCamera(Camera *camera) {_camera = camera;}
    void addLight(Light *light) {_lights.insert(light);}
    void addModel(Model *model) {_models.insert(model);}
    
    void render();
    void reset();
    
  private:
    GraphicTask getTemplateTask();
    void updateLightUniforms();
    
  public:
    static RenderPassMap& getRenderPassMap();
    static RenderPassPtr getRenderPass(const std::string &name);
    static void renderPasses(const RenderScheme &scheme);
    static void resetPasses();
  };
}



//#include "GraphicTask.h"
//#include "UniformMap.h"
//#include <vector>
//#include <string>
//#include <list>
//
//namespace fx {
//  class Camera;
//  class Model;
//  
//  struct RenderItem {
//    Model *model;
//    Camera *camera;
//    GraphicTask task;
//    
//    RenderItem(Model *model = nullptr): camera(0), model(model) {
//      task.uniforms = std::make_shared<UniformMap>();
//    }
//    RenderItem(const RenderItem &other) {*this = other;}
//    RenderItem(Camera *camera, Model *model);
//    RenderItem& operator=(const RenderItem &other);
//    
//    void update();
//    bool active() const;
//    
//    void setClearOperations(Camera *camera);
//    void setDefaultOperations();
//    
//    void setCamera(Camera *camera);
//    void setModel(Model *model);
//  };
//  typedef std::list<RenderItem> RenderItems;
//  
//  class RenderPass {
//    std::string _name;
//    Camera *_camera;
//    RenderItems _items;
//    
//  public:
//    RenderPass(const std::string name);
//    std::string name() {return _name;}
//    
//    void setCamera(Camera *camera);
//    Camera *camera() const {return _camera;}
//    
//    void addModel(Model *model);
//    void removeModel(Model *model);
//    
//    void update();
//    void render();
//  };
//  
//  class RenderPasses {
//  private:
//    std::vector<RenderPass> _renderPasses;
//    
//  public:
//    RenderPasses();
//    ~RenderPasses();
//    
//    void update();
//    void render();
//    RenderPass& operator[](const std::string &name);
//  };
//}

#endif /* RenderPass_h */
