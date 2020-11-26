//
//  MotionSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 11/25/20.
//  Copyright © 2020 Robert Crosby. All rights reserved.
//

#ifndef MotionSystem_h
#define MotionSystem_h

#include <FelixEngine/Quaternion.h>

namespace fx {

  class MotionSystem {
  protected:
    static MotionSystem *instance;
    
  public:
    static MotionSystem& getInstance() {return *instance;}
    
  public:
    MotionSystem() {}
    virtual ~MotionSystem() {}
    
    virtual quat getOrientation() = 0;
  };

}

#endif /* MotionSystem_h */
