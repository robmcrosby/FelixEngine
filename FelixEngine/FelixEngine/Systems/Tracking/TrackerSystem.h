//
//  TrackerSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 8/4/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef TrackerSystem_h
#define TrackerSystem_h

#include "Matrix.h"


namespace fx {
  class TrackerSystem {
  protected:
    static TrackerSystem *instance;
    
  public:
    static TrackerSystem& getInstance() {return *instance;}
    
  public:
    virtual ~TrackerSystem() {}
    
    virtual mat4 getCameraView() = 0;
    virtual mat4 getCameraProjection() = 0;
  };
}

#endif /* TrackerSystem_h */
