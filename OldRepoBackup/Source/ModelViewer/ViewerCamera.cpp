//
//  ViewerCamera.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 11/18/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "ViewerCamera.h"


using namespace std;

DEFINE_COMPONENT_ID(ViewerCamera)

ViewerCamera::ViewerCamera(fx::Scene *scene): fx::Camera(scene),
  mEventSystem(0),
  mMotionSystem(0),
  mOrbitView(0),
  mGyroView(0),
  mGraphicSystem(0),
  mMainWindow(0),
  mRenderMode(RENDER_MONO)
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
    
    if (mEventSystem)
    {
      mEventSystem->addHandler(this);
      mMainWindow = mGraphicSystem->getWindow(MAIN_WINDOW);
    }
    
    mMotionSystem = fx::FelixEngine::GetMotionSystem();
    if (mMotionSystem)
    {
      mMotionSystem->addHandler(this);
      
      mGyroView = new fx::GyroView(mScene);
      addChild(mGyroView);
      success &= mGyroView->init();
      mGyroView->setActive(false);
    }
    
    mOrbitView = new fx::OrbitView(mScene);
    mOrbitView->setDistance(2.5f);
    addChild(mOrbitView);
    
    delete mView;
    mView = mOrbitView;
    success &= mOrbitView->init();
  }
  return success;
}

void ViewerCamera::setToXml(const fx::XMLTree::Node &node)
{
  fx::Camera::setToXml(node);
  if (mRenderSlots)
  {
    for (fx::RenderSlot *slot : *mRenderSlots)
    {
      if (slot && slot->taskType() == fx::GRAPHIC_TASK_PASS)
      {
        fx::RenderSlot *leftPass  = new fx::RenderSlot(*slot);
        fx::RenderSlot *rightPass = new fx::RenderSlot(*slot);
        
        // Add the mono pass.
        mMonoPasses.push_back(slot);
        
        // Setup and add the left pass copy.
        leftPass->setProjectionFlags(fx::PROJ_LEFT | fx::PROJ_SPLIT);
        leftPass->setStereoType(fx::STEREO_LEFT);
        mRenderSlots->addSlot(leftPass);
        mStereoPasses.push_back(leftPass);
        
        // Setup and add the right pass copy.
        rightPass->setProjectionFlags(fx::PROJ_RIGHT | fx::PROJ_SPLIT);
        rightPass->setStereoType(fx::STEREO_RIGHT);
        mRenderSlots->addSlot(rightPass);
        mStereoPasses.push_back(rightPass);
      }
    }
    setRenderMode(RENDER_MONO);
  }
}

void ViewerCamera::setRenderMode(RENDER_MODE mode)
{
  int rotationFlag = mode == RENDER_STEREO_LEFT ? fx::PROJ_ROT_CW : fx::PROJ_ROT_CCW;
  mRenderMode = mode;
  
  if (mode == RENDER_MONO)
  {
    // Set to Render Mono
    for (fx::RenderSlot *slot : mMonoPasses)
      slot->enable();
    for (fx::RenderSlot *slot : mStereoPasses)
      slot->disable();
  }
  else
  {
    // Set to Render Stereo
    for (fx::RenderSlot *slot : mMonoPasses)
      slot->disable();
    for (fx::RenderSlot *slot : mStereoPasses)
    {
      int projFlags = (slot->projectionFlags() & ~fx::PROJ_ROTATION) | rotationFlag;
      slot->setProjectionFlags(projFlags);
      slot->enable();
    }
  }
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
  if (mOrbitView && mOrbitView == mView)
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
  if (mOrbitView && mOrbitView == mView)
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
    if (mOrbitView == mView && (gravity.x > 0.8f || gravity.x < -0.8f))
    {
      mGyroView->setDistance(mOrbitView->distance());
      mGyroView->setActive(true);
      mView = mGyroView;
      
      fx::quat orientation(fx::vec3(1.0f, 0.0f, 0.0f), -90.0f*fx::DegToRad);
      orientation *= fx::quat(fx::vec3(0.0f, 0.0f, 1.0f), (mOrbitView->longitude()-180.0f)*fx::DegToRad);
      if (gravity.x > 0.8f)
      {
        mGyroView->setUpAxis(fx::vec3(-1.0, 0.0f, 0.0f));
        setRenderMode(RENDER_STEREO_RIGHT);
      }
      else
      {
        mGyroView->setUpAxis(fx::vec3(1.0, 0.0f, 0.0f));
        setRenderMode(RENDER_STEREO_LEFT);
      }
      mGyroView->setOrientation(orientation);
      mProjection->setZeroDistance(mOrbitView->distance());
    }
    else if (mGyroView->active() && gravity.y < -0.8f)
    {
      mGyroView->setActive(false);
      mView = mOrbitView;
      setRenderMode(RENDER_MONO);
    }
  }
}
