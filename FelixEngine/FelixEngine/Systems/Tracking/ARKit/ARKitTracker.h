//
//  ARKitTracker.h
//  FelixEngine
//
//  Created by Robert Crosby on 8/4/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "TrackerSystem.h"

#ifndef ARKitTracker_h
#define ARKitTracker_h

namespace fx {
  
  /** iOS File System */
  class ARKitTracker: public TrackerSystem {
  public:
    ARKitTracker();
    virtual ~ARKitTracker();
  };
  
}

#endif /* ARKitTracker_h */
