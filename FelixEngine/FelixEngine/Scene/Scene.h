//
//  Scene.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Scene_h
#define Scene_h

#include <stdio.h>

namespace fx {
  class Model;
  class Material;
  class Light;
  class Camera;
  
  class Scene {
  public:
    Scene();
    virtual ~Scene();
    
    virtual void update();
    virtual void render();
  };
  
}

#endif /* Scene_h */
