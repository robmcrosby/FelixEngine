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
  class TextureBuffer;
  
  enum TRACKING_STATUS {
    TRACKING_NOT_AVALIBLE,
    TRACKING_LIMITED,
    TRACKING_NORMAL,
  };
  
  class TrackerSystem {
  protected:
    static TrackerSystem *instance;
    
  protected:
    TRACKING_STATUS _trackingStatus;
    
  public:
    static TrackerSystem& getInstance() {return *instance;}
    
  public:
    TrackerSystem(): _trackingStatus(TRACKING_NOT_AVALIBLE) {}
    virtual ~TrackerSystem() {}
    
    TRACKING_STATUS trackingStatus() const {return _trackingStatus;}
    
    virtual mat4 getCameraView() = 0;
    virtual mat4 getCameraProjection() = 0;
    
    virtual TextureBuffer* getCameraImageY() = 0;
    virtual TextureBuffer* getCameraImageCbCr() = 0;
  };
}

#endif /* TrackerSystem_h */
