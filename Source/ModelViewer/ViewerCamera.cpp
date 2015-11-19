//
//  ViewerCamera.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 11/18/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "ViewerCamera.h"

using namespace std;

DEFINE_OBJECT_ID(ViewerCamera)

ViewerCamera::ViewerCamera(fx::Scene *scene): fx::Camera(scene)
{
  setEventFlags(fx::EVENT_TOUCH | fx::EVENT_MOUSE);
  cout << "Created Viewer Camera" << endl;
}

bool ViewerCamera::init()
{
  bool success = Camera::init();
  if (success)
  {
    mEventSystem = fx::FelixEngine::GetEventSystem();
    if (mEventSystem)
      mEventSystem->addHandler(this);
    mOrbitView = dynamic_cast<fx::OrbitView*>(view());
  }
  return success;
}

void ViewerCamera::handle(const fx::Event &event)
{
//  if (event == fx::EVENT_MOUSE)
//    handleMouseEvent(event);
  if (event == fx::EVENT_TOUCH)
    handleTouchEvent(event);
}

void ViewerCamera::handleMouseEvent(const fx::Event &event)
{
  if (mOrbitView && event == fx::EVENT_MOUSE_MOVE && (event.mouseData().buttons & fx::MOUSE_BUTTON_LEFT))
  {
    mOrbitView->setLongitude(event.mouseData().delta.x*-0.5f + mOrbitView->longitude());
    mOrbitView->setLatitude(event.mouseData().delta.y*0.5f + mOrbitView->latitude());
  }
}

void ViewerCamera::handleTouchEvent(const fx::Event &event)
{
  if (mOrbitView && event == fx::EVENT_TOUCH_MOVE)
  {
    cout << "Touch: " << event.touchData().index << endl;
    mOrbitView->setLongitude(event.touchData().delta.x*200.0f + mOrbitView->longitude());
    mOrbitView->setLatitude(event.touchData().delta.y*-200.0f + mOrbitView->latitude());
  }
}
