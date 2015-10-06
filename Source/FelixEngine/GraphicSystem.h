//
//  GraphicSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GraphicSystem_h
#define GraphicSystem_h

#include "System.h"
#include "Vector.h"


namespace fx
{
  struct Window
  {
    virtual ~Window() {}
    virtual bool loadResizable(const std::string &title, ivec2 pos, ivec2 size) = 0;
  };
  
  /**
   *
   */
  class GraphicSystem: public System
  {
  public:
    GraphicSystem(): System(SYSTEM_GRAPHICS) {}
    virtual ~GraphicSystem() {}
    
    virtual Window* getWindow(const std::string &name) = 0;
    
    static GraphicSystem* CreateOpenGLContext();
  };
}


#endif /* GPU_System_h */