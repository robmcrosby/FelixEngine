//
//  ARPlaneDetect.h
//  ARPlaneDetect
//
//  Created by Robert Crosby on 8/9/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef ARPlaneDetect_h
#define ARPlaneDetect_h

#import <FelixEngine/Application.h>
#import <FelixEngine/Scene.h>
#import <FelixEngine/Model.h>
#import <FelixEngine/RenderPass.h>
#import <FelixEngine/TrackerSystem.h>
#import <map>

class ARPlaneDetect: public fx::Application, public fx::TrackerDelegate {
private:
  fx::TrackerSystem *_trackerSystem;
  fx::Scene _scene;
  fx::ModelPtr _planes;
  
public:
  ARPlaneDetect();
  virtual ~ARPlaneDetect();
  
  virtual void initalize();
  virtual void update(float td);
  
  virtual void handle(const fx::Event &event);
  
  void handleTouchDown(const fx::Event &event);
  
  void setTrackerSystem(fx::TrackerSystem *tracker);
  virtual void trackedPlaneAdded(fx::ARPlane plane);
  virtual void trackedPlaneUpdated(fx::ARPlane plane);
  virtual void trackedPlaneRemoved(fx::ARPlane plane);
};

#endif /* ARPlaneDetect_h */
