//
//  ARKitTracker.h
//  FelixEngine
//
//  Created by Robert Crosby on 8/4/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "TrackerSystem.h"
#include "Macros.h"

#ifndef ARKitTracker_h
#define ARKitTracker_h

OBJC_CLASS(ARSession)
OBJC_CLASS(ARDelegate)
OBJC_CLASS(ARAnchor)

namespace fx {
  class MetalGraphics;
  
  /** iOS File System */
  class ARKitTracker: public TrackerSystem {
  private:
    MetalGraphics *_graphics;
    ARSession     *_arSession;
    ARDelegate    *_arDelegate;
    ARAnchor      *_firstAnchor;
    
  public:
    ARKitTracker();
    virtual ~ARKitTracker();
    
    bool initalize(MetalGraphics *graphics);
    
    virtual mat4 getCameraView();
    virtual mat4 getCameraProjection();
    
  public:
    void arSessionFailed();
    void arSessionInterupted();
    void arSessionInteruptEnd();
    void setTrackingStatus(TRACKING_STATUS status);
  };
  
}

#endif /* ARKitTracker_h */
