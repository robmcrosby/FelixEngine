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
#include <vector>
#include <string>
#include <list>

namespace fx {
  class Camera;
  class Model;
  
  struct RenderItem {
    Model *model;
    UniformMap uniforms;
    GraphicTask task;
    
    RenderItem(Model *model = nullptr): model(model) {task.uniforms = &uniforms;}
    RenderItem(const RenderItem &other) {*this = other;}
    RenderItem& operator=(const RenderItem &other);
    bool active() const;
    void setClearOperations(Camera *camera);
    void setDefaultOperations();
  };
  typedef std::list<RenderItem> RenderItems;
  
  class RenderPass {
    std::string _name;
    Camera *_camera;
    RenderItems _items;
    
  public:
    RenderPass(const std::string name);
    std::string name() {return _name;}
    
    void setCamera(Camera *camera);
    Camera *camera() const {return _camera;}
    
    void addModel(Model *model);
    void removeModel(Model *model);
    
    void update();
    void render();
  };
  
  class RenderPasses {
  private:
    std::vector<RenderPass> _renderPasses;
    
  public:
    RenderPasses();
    ~RenderPasses();
    
    void update();
    void render();
    RenderPass& operator[](const std::string &name);
  };
}

#endif /* RenderPass_h */
