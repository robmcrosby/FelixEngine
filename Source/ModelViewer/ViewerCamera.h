//
//  ViewerCamera.h
//  FelixEngine
//
//  Created by Robert Crosby on 11/18/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef ViewerCamera_h
#define ViewerCamera_h

#include <FelixEngine/Camera.h>
#include <FelixEngine/OrbitView.h>
#include <FelixEngine/GyroView.h>
#include <FelixEngine/GraphicSystem.h>


class ViewerCamera: public fx::Camera
{
public:
  ViewerCamera(fx::Scene *scene);
  virtual ~ViewerCamera() {}
  
  virtual bool init();
  virtual void handle(const fx::Event &event);
  
private:
  void handleMouseEvent(const fx::Event &event);
  void handleTouchEvent(const fx::Event &event);
  void handleMotionEvent(const fx::Event &event);
  
  fx::EventSystem  *mEventSystem;
  fx::MotionSystem *mMotionSystem;
  
  fx::OrbitView *mOrbitView;
  fx::GyroView  *mGyroView;
  
  fx::GraphicSystem *mGraphicSystem;
  fx::Window *mMainWindow;
};

#endif /* ViewerCamera_h */
