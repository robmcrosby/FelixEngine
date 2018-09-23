//
//  TrackerSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 8/4/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef TrackerSystem_h
#define TrackerSystem_h

#include "Quaternion.h"
#include "Graphics.h"
#include <vector>


namespace fx {
  class TextureBuffer;
  
  enum TRACKING_STATUS {
    TRACKING_NOT_AVALIBLE,
    TRACKING_LIMITED,
    TRACKING_NORMAL,
  };
  
  enum FEATURE_TRACKING_FLAGS {
    FEATURE_TRACKING_NONE              = 0x00,
    FEATURE_TRACKING_POINTS            = 0x01,
    FEATURE_TRACKING_PLANES_HORIZONTAL = 0x02,
    FEATURE_TRACKING_PLANES_VERTICAL   = 0x04,
    FEATURE_TRACKING_GEOMETRY          = 0x08,
  };
  
  struct ARPlane {
    std::string uuid;
    mat4 transform;
    vec3 center;
    vec3 position;
    quat rotation;
    vec2 extent;
  };
  typedef std::vector<ARPlane> ARPlanes;
  
  struct TrackerDelegate {
    virtual ~TrackerDelegate() {}
    
    virtual void trackedPlaneAdded(ARPlane plane) = 0;
    virtual void trackedPlaneUpdated(ARPlane plane) = 0;
    virtual void trackedPlaneRemoved(ARPlane plane) = 0;
  };
  
  struct ARPoint {
    ARPoint(): identifier(0) {}
    
    long identifier;
    fx::vec3 position;
  };
  typedef std::vector<ARPoint> ARPoints;
  
  class TrackerSystem {
  protected:
    static TrackerSystem *instance;
    
  protected:
    TRACKING_STATUS _trackingStatus;
    TrackerDelegate *_delegate;
    
    u_int32_t _freatureTrackingFlags;
    
  public:
    static TrackerSystem& getInstance() {return *instance;}
    
  public:
    TrackerSystem(): _trackingStatus(TRACKING_NOT_AVALIBLE), _freatureTrackingFlags(0) {
      _delegate = nullptr;
    }
    virtual ~TrackerSystem() {}
    
    TRACKING_STATUS trackingStatus() const {return _trackingStatus;}
    
    virtual mat4 getCameraView() = 0;
    virtual mat4 getCameraProjection() = 0;
    virtual mat4 getImageTransform() = 0;
    
    virtual TextureBufferPtr getCameraImageY() = 0;
    virtual TextureBufferPtr getCameraImageCbCr() = 0;
    
    void setDelegate(TrackerDelegate *delegate) {_delegate = delegate;}
    
    virtual void enableFeatureTracking(unsigned int featureFlags) {_freatureTrackingFlags |= featureFlags;}
    virtual void disableFeatureTracking(unsigned int featureFlags) {_freatureTrackingFlags &= ~featureFlags;}
    
    void enablePointCloud() {enableFeatureTracking(FEATURE_TRACKING_POINTS);}
    void disablePointCloud() {disableFeatureTracking(FEATURE_TRACKING_POINTS);}
    bool pointCloudEnabled() {return _freatureTrackingFlags & FEATURE_TRACKING_POINTS;}
    virtual const ARPoints& getPointCloud() const = 0;
    
    void enablePlaneDetection() {enableFeatureTracking(FEATURE_TRACKING_PLANES_VERTICAL | FEATURE_TRACKING_PLANES_HORIZONTAL);}
    void disablePlaneDetection() {disableFeatureTracking(FEATURE_TRACKING_PLANES_VERTICAL | FEATURE_TRACKING_PLANES_HORIZONTAL);}
    bool planeDetectionEnabled() const {return _freatureTrackingFlags & (FEATURE_TRACKING_PLANES_VERTICAL | FEATURE_TRACKING_PLANES_HORIZONTAL);}
    virtual const ARPlanes& trackedPlanes() const = 0;
    
    void enableHorizontalPlaneDetection() {enableFeatureTracking(FEATURE_TRACKING_PLANES_HORIZONTAL);}
    void disableHorizontalPlaneDetection() {disableFeatureTracking(FEATURE_TRACKING_PLANES_HORIZONTAL);}
    bool horizontalPlaneDetectionEnabled() {return _freatureTrackingFlags & FEATURE_TRACKING_PLANES_HORIZONTAL;}
    
    void enableVerticalPlaneDetection() {enableFeatureTracking(FEATURE_TRACKING_PLANES_VERTICAL);}
    void disableVerticalPlaneDetection() {disableFeatureTracking(FEATURE_TRACKING_PLANES_VERTICAL);}
    bool verticalPlaneDetectionEnabled() {return _freatureTrackingFlags & FEATURE_TRACKING_PLANES_VERTICAL;}
  };
}

#endif /* TrackerSystem_h */
