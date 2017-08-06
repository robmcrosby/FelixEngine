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
OBJC_CLASS(ARFrame)
OBJC_CLASS(ARDelegate)
OBJC_CLASS(ARAnchor)

namespace fx {
  class MetalGraphics;
  class MetalTextureBuffer;
  
  /** iOS File System */
  class ARKitTracker: public TrackerSystem {
  private:
    MetalGraphics *_graphics;
    ARSession     *_arSession;
    ARDelegate    *_arDelegate;
    ARAnchor      *_firstAnchor;
    
    MetalTextureBuffer *_cameraImageY;
    MetalTextureBuffer *_cameraImageCbCr;
    
  public:
    ARKitTracker();
    virtual ~ARKitTracker();
    
    bool initalize(MetalGraphics *graphics);
    
    virtual mat4 getCameraView();
    virtual mat4 getCameraProjection();
    
    virtual TextureBuffer* getCameraImageY();
    virtual TextureBuffer* getCameraImageCbCr();
    
  public:
    void arSessionFailed();
    void arSessionInterupted();
    void arSessionInteruptEnd();
    void arSessionUpdateFrame(ARFrame *frame);
    void setTrackingStatus(TRACKING_STATUS status);
  };
  
}

#endif /* ARKitTracker_h */
