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
#include "Graphics.h"
#include <vector>


namespace fx {
  class TextureBuffer;
  
  enum TRACKING_STATUS {
    TRACKING_NOT_AVALIBLE,
    TRACKING_LIMITED,
    TRACKING_NORMAL,
  };
  
  struct TrackedPlane {
    std::string uuid;
    mat4 transform;
    vec3 center;
    vec2 extent;
  };
  typedef std::vector<TrackedPlane> TrackedPlanes;
  
  struct TrackerDelegate {
    virtual ~TrackerDelegate() {}
    
    virtual void trackedPlaneAdded(TrackedPlane plane) = 0;
    virtual void trackedPlaneUpdated(TrackedPlane plane) = 0;
    virtual void trackedPlaneRemoved(TrackedPlane plane) = 0;
  };
  
  struct ARPoint {
    ARPoint(): identifier(0) {}
    
    long identifier;
    fx::vec3 position;
  };
  typedef std::vector<ARPoint> ARPointVector;
  
  class TrackerSystem {
  protected:
    static TrackerSystem *instance;
    
  protected:
    TRACKING_STATUS _trackingStatus;
    TrackerDelegate *_delegate;
    
    bool _planeDetectionEnabled;
    
  public:
    static TrackerSystem& getInstance() {return *instance;}
    
  public:
    TrackerSystem(): _trackingStatus(TRACKING_NOT_AVALIBLE) {
      _delegate = nullptr;
      _planeDetectionEnabled = false;
    }
    virtual ~TrackerSystem() {}
    
    TRACKING_STATUS trackingStatus() const {return _trackingStatus;}
    
    virtual mat4 getCameraView() = 0;
    virtual mat4 getCameraProjection() = 0;
    virtual mat4 getImageTransform() = 0;
    
    virtual void enablePointCloud(bool enable) = 0;
    virtual const ARPointVector& getPointCloud() const = 0;
    
    virtual TextureBufferPtr getCameraImageY() = 0;
    virtual TextureBufferPtr getCameraImageCbCr() = 0;
    
    void setDelegate(TrackerDelegate *delegate) {_delegate = delegate;}
    
    virtual void enablePlaneDetection(bool enable = true) {_planeDetectionEnabled = enable;}
    bool planeDetectionEnabled() const {return _planeDetectionEnabled;}
    
    virtual const TrackedPlanes& trackedPlanes() const = 0;
  };
}

#endif /* TrackerSystem_h */
