//
//  RenderPasses.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/8/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef RenderPasses_h
#define RenderPasses_h

#include <vector>
#include <string>

namespace fx {
  class Camera;
  class Model;
  
  typedef std::vector<Model*> Models;
  
  struct RenderPass {
    std::string name;
    int layer;
    Camera *camera;
    Models models;
    
    RenderPass(): layer(0), camera(nullptr) {}
    RenderPass(const std::string name): name(name), layer(0), camera(nullptr) {}
  };
  
  class RenderPasses {
  private:
    std::vector<RenderPass> _renderPasses;
    
  public:
    RenderPasses();
    ~RenderPasses();
    
    void render();
    RenderPass& operator[](const std::string &name);
  };
}

#endif /* RenderPasses_h */
