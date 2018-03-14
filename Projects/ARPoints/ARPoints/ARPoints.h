//
//  ARPoints.h
//  ARPoints
//
//  Created by Robert Crosby on 2/18/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef ARPoints_h
#define ARPoints_h

#include <unordered_map>
#include <FelixEngine/Application.h>
#include <FelixEngine/Scene.h>
#include <FelixEngine/RenderPass.h>
#include <FelixEngine/TrackerSystem.h>

class ARPoints: public fx::Application {
  private:
  fx::RenderScheme _renderScheme;
  fx::Scene _scene;
  fx::ModelPtr _model;
  fx::TrackerSystem *_tracker;
  
  std::unordered_map<long, fx::vec3> _pointCloud;
  
  public:
  ARPoints();
  virtual ~ARPoints();
  
  virtual void initalize();
  virtual void update(float td);
  
  void setTracker(fx::TrackerSystem *tracker) {_tracker = tracker;}
};

#endif /* ARPoints_h */
