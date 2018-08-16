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
  fx::Scene _scene;
  fx::ModelPtr _model;
  
  std::map<std::string, fx::TrackedPlane> _trackedPlanes;
  
public:
  ARPlaneDetect();
  virtual ~ARPlaneDetect();
  
  virtual void initalize();
  virtual void update(float td);
  
  virtual void trackedPlaneAdded(fx::TrackedPlane plane);
  virtual void trackedPlaneUpdated(fx::TrackedPlane plane);
  virtual void trackedPlaneRemoved(fx::TrackedPlane plane);
};

#endif /* ARPlaneDetect_h */
