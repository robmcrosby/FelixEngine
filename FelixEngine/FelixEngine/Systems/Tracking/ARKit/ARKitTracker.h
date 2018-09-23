//
//  ARKitTracker.h
//  FelixEngine
//
//  Created by Robert Crosby on 8/4/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "TrackerSystem.h"
#include "Graphics.h"
#include "UniformMap.h"
#include "Macros.h"

#ifndef ARKitTracker_h
#define ARKitTracker_h

OBJC_CLASS(ARSession)
OBJC_CLASS(ARFrame)
OBJC_CLASS(ARDelegate)
OBJC_CLASS(ARAnchor)
OBJC_CLASS(ARPlaneAnchor)

namespace fx {
  class MetalGraphics;
  class MetalTextureBuffer;
  
  /** iOS File System */
  class ARKitTracker: public TrackerSystem {
  private:
    MetalGraphics *_graphics;
    ARSession     *_arSession;
    ARDelegate    *_arDelegate;
    
    ARPoints _pointCloud;
    
    std::shared_ptr<MetalTextureBuffer> _cameraImageY;
    std::shared_ptr<MetalTextureBuffer> _cameraImageCbCr;
    
    UniformsPtr _uniformMap;
    //GraphicTask _task;
    
    ARPlanes _trackedPlanes;
    
  public:
    ARKitTracker();
    virtual ~ARKitTracker();
    
    bool initalize(MetalGraphics *graphics);
    
    virtual mat4 getCameraView();
    virtual mat4 getCameraProjection();
    virtual mat4 getImageTransform();
    
    virtual void enableFeatureTracking(unsigned int featureFlags);
    virtual void disableFeatureTracking(unsigned int featureFlags);
    
    virtual const ARPoints& getPointCloud() const;
    
    virtual TextureBufferPtr getCameraImageY();
    virtual TextureBufferPtr getCameraImageCbCr();
    
    virtual const ARPlanes& trackedPlanes() const;
    
  public:
    void arSessionFailed();
    void arSessionInterupted();
    void arSessionInteruptEnd();
    void arSessionUpdateFrame(ARFrame *frame);
    void setTrackingStatus(TRACKING_STATUS status);
    
    void planeAnchorAdded(ARPlaneAnchor *anchor);
    void planeAnchorUpdated(ARPlaneAnchor *anchor);
    void planeAnchorRemoved(ARPlaneAnchor *anchor);
    
  private:
    void updateTrackedPlane(const ARPlane &plane);
  };
  
}

#endif /* ARKitTracker_h */
