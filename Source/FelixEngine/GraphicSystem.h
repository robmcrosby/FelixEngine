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


namespace fx
{
  struct Window
  {
  };
  
  /**
   *
   */
  class GraphicSystem: public System
  {
  public:
    GraphicSystem(): System(SYSTEM_GRAPHICS) {}
    virtual ~GraphicSystem() {}
    
    virtual Window* createResizableWindow() = 0;
    
    static GraphicSystem* CreateOpenGLContext();
    static GraphicSystem* CreateMetalContext();
  };
}


#endif /* GPU_System_h */
