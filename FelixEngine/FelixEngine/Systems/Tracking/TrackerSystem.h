//
//  TrackerSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 8/4/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef TrackerSystem_h
#define TrackerSystem_h

#include <string>

namespace fx {
  class TrackerSystem {
  protected:
    static TrackerSystem *instance;
    
  public:
    static TrackerSystem& getInstance() {return *instance;}
    
  public:
    virtual ~TrackerSystem() {}
  };
}

#endif /* TrackerSystem_h */
