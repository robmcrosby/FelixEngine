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
    void updateLightUniforms();
    
  public:
    static RenderPassMap& getRenderPassMap();
    static RenderPassPtr getRenderPass(const std::string &name);
    static void renderPasses(const RenderScheme &scheme);
    static void resetPasses();
  };
}

#endif /* RenderPass_h */
