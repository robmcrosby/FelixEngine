//
//  GpuSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GpuSystem_h
#define GpuSystem_h

#include "System.h"


namespace fx
{
  struct GPUWindow
  {
  };
  
  /**
   *
   */
  class GpuSystem: public System
  {
  public:
    GpuSystem(): System(SYSTEM_GPU) {}
    virtual ~GpuSystem() {}
    
    virtual GPUWindow* createResizableWindow() = 0;
    
    static GpuSystem* CreateOpenGLSystem();
    static GpuSystem* CreateMetalSystem();
  };
}


#endif /* GpuSystem_h */
