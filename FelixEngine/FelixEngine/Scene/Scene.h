//
//  Scene.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Scene_h
#define Scene_h

#include "RenderPasses.h"

namespace fx {
  
  class Scene {
  protected:
    RenderPasses _renderPasses;
    
  public:
    Scene();
    virtual ~Scene();
    
    virtual void update();
    virtual void render();
    
    RenderPasses& renderPasses() {return _renderPasses;}
  };
  
}

#endif /* Scene_h */
