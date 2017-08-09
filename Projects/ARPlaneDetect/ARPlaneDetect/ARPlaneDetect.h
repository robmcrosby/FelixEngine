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
#import <FelixEngine/Graphics.h>
#import <FelixEngine/GraphicTask.h>

class ARPlaneDetect: public fx::Application {
private:
  fx::Scene _scene;
  
public:
  ARPlaneDetect();
  virtual ~ARPlaneDetect();
  
  virtual void initalize();
  virtual void update();
  virtual void render();
};

#endif /* ARPlaneDetect_h */
