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
  
  fx::EventSystem *mEventSystem;
  fx::OrbitView *mOrbitView;
};

#endif /* ViewerCamera_h */
