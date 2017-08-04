//
//  ARTracker.h
//  ARTracker
//
//  Created by Robert Crosby on 8/4/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef ARTracker_h
#define ARTracker_h

#import <FelixEngine/Application.h>
#import <FelixEngine/Scene.h>
#import <FelixEngine/Graphics.h>
#import <FelixEngine/GraphicTask.h>

class ARTracker: public fx::Application {
private:
  fx::Scene _scene;
  fx::Model *_model;
  
public:
  ARTracker();
  virtual ~ARTracker();
  
  virtual void initalize();
  virtual void update();
  virtual void render();
};

#endif /* ARTracker_h */
