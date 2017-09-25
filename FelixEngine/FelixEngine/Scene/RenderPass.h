//
//  RenderPass.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/8/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef RenderPass_h
#define RenderPass_h

#include "GraphicTask.h"
#include "UniformMap.h"
#include "Camera.h"
#include "Model.h"
#include "Light.h"


namespace fx {
  class RenderPass;
  typedef std::shared_ptr<RenderPass> RenderPassPtr;
  
  class RenderPass {
  private:
    CameraPtr _camera;
    std::vector<LightPtr> _lights;
    std::vector<ModelPtr> _models;
    
  public:
    RenderPass();
    ~RenderPass();
    
    void setCamera(CameraPtr &camera) {_camera = camera;}
    void addLight(LightPtr &light) {_lights.push_back(light);}
    void addModel(ModelPtr &model) {_models.push_back(model);}
    
    void render();
    void clear();
    
  public:
    static RenderPassPtr getMainRenderPass();
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
