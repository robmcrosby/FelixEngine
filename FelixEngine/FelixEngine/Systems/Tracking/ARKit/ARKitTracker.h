//
//  ARKitTracker.h
//  FelixEngine
//
//  Created by Robert Crosby on 8/4/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include <FelixEngine/TrackerSystem.h>
#include <FelixEngine/Graphics.h>
#include <FelixEngine/UniformMap.h>
#include <FelixEngine/Macros.h>

#ifndef ARKitTracker_h
#define ARKitTracker_h

OBJC_CLASS(ARFrame)
OBJC_CLASS(ARDelegate)
OBJC_CLASS(ARPlaneAnchor)
OBJC_CLASS(AREnvironmentProbeAnchor);

namespace fx {
  class MetalGraphics;
  class MetalTextureBuffer;
  
  /** AR Kit Tracker  */
  class ARKitTracker: public TrackerSystem {
  private:
    MetalGraphics *_graphics;
    ARDelegate    *_arDelegate;
    
    ARPoints _pointCloud;
    
    std::shared_ptr<MetalTextureBuffer> _cameraImageY;
    std::shared_ptr<MetalTextureBuffer> _cameraImageCbCr;
    std::shared_ptr<MetalTextureBuffer> _environmentMap;
    
    UniformsPtr _uniformMap;
    
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
    virtual TextureBufferPtr getEnvironmentMap();
    
    virtual ARHitResults hitTest(const Touch &touch);
    
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
    
    void environmentProbeAdded(AREnvironmentProbeAnchor *anchor);
    void environmentProbeUpdated(AREnvironmentProbeAnchor *anchor);
    void environmentProbeRemoved(AREnvironmentProbeAnchor *anchor);
    
    ARPlane planeForAnchor(ARPlaneAnchor *anchor);
    
  private:
    void updateTrackedPlane(const ARPlane &plane);
  };
  
}

#endif /* ARKitTracker_h */
