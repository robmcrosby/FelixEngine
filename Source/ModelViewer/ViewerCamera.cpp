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
  #if TARGET_OS_IPHONE
  setEventFlags(fx::EVENT_TOUCH);
  #else
  setEventFlags(fx::EVENT_MOUSE);
  #endif
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
  if (event == fx::EVENT_MOUSE)
    handleMouseEvent(event);
  if (event == fx::EVENT_TOUCH)
    handleTouchEvent(event);
}

void ViewerCamera::handleMouseEvent(const fx::Event &event)
{
  if (mOrbitView)
  {
    if (event == fx::EVENT_MOUSE_WHEEL)
      mOrbitView->setDistance(mOrbitView->distance() + event.mouseData().delta.y*0.1f);
    else if (event == fx::EVENT_MOUSE_MOVE && (event.mouseData().buttons & fx::MOUSE_BUTTON_LEFT))
    {
      mOrbitView->setLongitude(event.mouseData().delta.x*-0.5f + mOrbitView->longitude());
      mOrbitView->setLatitude(event.mouseData().delta.y*0.5f + mOrbitView->latitude());
    }
  }
}

void ViewerCamera::handleTouchEvent(const fx::Event &event)
{
  if (mOrbitView)
  {
    if (event == fx::EVENT_TOUCH_GESTURE)
      mOrbitView->setDistance(mOrbitView->distance() - event.touchData().delta.y*8.0f);
    if (event == fx::EVENT_TOUCH_MOVE)
    {
      mOrbitView->setLongitude(mOrbitView->longitude() - event.touchData().delta.x*200.0f);
      mOrbitView->setLatitude(mOrbitView->latitude() + event.touchData().delta.y*200.0f);
    }
  }
}
