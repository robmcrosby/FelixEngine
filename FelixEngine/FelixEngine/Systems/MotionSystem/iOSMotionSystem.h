//
//  iOSMotionSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 11/25/20.
//  Copyright © 2020 Robert Crosby. All rights reserved.
//

#include <FelixEngine/MotionSystem.h>
#include "Macros.h"


#ifndef iOSMotionSystem_h
#define iOSMotionSystem_h

OBJC_CLASS(CMMotionManager)


namespace fx {

  class iOSMotionSystem: public MotionSystem {
  private:
    CMMotionManager *_motionManager;
    
  public:
    iOSMotionSystem();
    virtual ~iOSMotionSystem();
    
    bool initalize(double interval);
    void stop();
    
    virtual quat getOrientation();
  };
}

#endif /* iOSMotionSystem_h */
