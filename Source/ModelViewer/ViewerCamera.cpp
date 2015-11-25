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

ViewerCamera::ViewerCamera(fx::Scene *scene): fx::Camera(scene),
mEventSystem(0), mMotionSystem(0), mOrbitView(0), mGyroView(0), mGraphicSystem(0)
{
  #if TARGET_OS_IPHONE
  setEventFlags(fx::EVENT_TOUCH | fx::EVENT_MOTION);
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
    mGraphicSystem = fx::FelixEngine::GetGraphicSystem();
    
    mRenderSlots->setGlobal("ViewRot", fx::mat4());
    
    if (mEventSystem)
      mEventSystem->addHandler(this);
    
    mMotionSystem = fx::FelixEngine::GetMotionSystem();
    if (mMotionSystem)
    {
      mMotionSystem->addHandler(this);
      
      mGyroView = new fx::GyroView(this);
      success &= mGyroView->init();
      mGyroView->setActive(false);
    }
    
    mOrbitView = new fx::OrbitView(this);
    
    delete mView;
    mView = mOrbitView;
    success &= mOrbitView->init();
  }
  return success;
}

void ViewerCamera::handle(const fx::Event &event)
{
  if (event == fx::EVENT_MOUSE)
    handleMouseEvent(event);
  else if (event == fx::EVENT_TOUCH)
    handleTouchEvent(event);
  else if (event == fx::EVENT_MOTION)
    handleMotionEvent(event);
}

void ViewerCamera::handleMouseEvent(const fx::Event &event)
{
  if (mOrbitView && mOrbitView->active())
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
  if (mOrbitView && mOrbitView->active())
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

void ViewerCamera::handleMotionEvent(const fx::Event &event)
{
  if (mOrbitView && mGyroView)
  {
    fx::vec3 gravity = event.motionData().gravity;
    if (mOrbitView->active() && (gravity.x > 0.8f || gravity.x < -0.8f))
    {
      mGyroView->setDistance(mOrbitView->distance());
      mGyroView->setActive(true);
      mOrbitView->setActive(false);
      
      fx::quat orientation(fx::vec3(1.0f, 0.0f, 0.0f), -90.0f*fx::DegToRad);
      orientation *= fx::quat(fx::vec3(0.0f, 0.0f, 1.0f), (mOrbitView->longitude()-180.0f)*fx::DegToRad);
      if (gravity.x > 0.8f)
      {
        mRenderSlots->setGlobal("ViewRot", fx::mat4::RotZ(90*fx::DegToRad));
        mGyroView->setUpAxis(fx::vec3(-1.0, 0.0f, 0.0f));
      }
      else
      {
        mRenderSlots->setGlobal("ViewRot", fx::mat4::RotZ(-90*fx::DegToRad));
        mGyroView->setUpAxis(fx::vec3(1.0, 0.0f, 0.0f));
      }
      mGyroView->setOrientation(orientation);
      mGraphicSystem->setStereoFlags(fx::STEREO_BINARY);
    }
    else if (mGyroView->active() && gravity.y < -0.8f)
    {
      mOrbitView->setActive(true);
      mGyroView->setActive(false);
      mRenderSlots->setGlobal("ViewRot", fx::mat4());
      mGraphicSystem->setStereoFlags(fx::STEREO_MONO);
    }
  }
}
