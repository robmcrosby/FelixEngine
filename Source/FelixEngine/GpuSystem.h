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
  /**
   *
   */
  class GpuSystem: public System
  {
  public:
    GpuSystem(): System(SYSTEM_GPU) {}
    virtual ~GpuSystem() {}
  };
}


#endif /* GpuSystem_h */
