//
//  SDLEventSystem.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 11/3/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "SDLEventSystem.h"

using namespace fx;
using namespace std;

SDLEventSystem::SDLEventSystem(): mTouchLock(0), mLastDistance(0.0f)
{
  setEventFlags(EVENT_APP_UPDATE);
}

SDLEventSystem::~SDLEventSystem()
{
}

bool SDLEventSystem::setToXml(const XMLTree::Node *node)
{
  return true;
}

void SDLEventSystem::pollEvents()
{
  if (mUpdateMutex.tryLock())
  {
    SDL_Event event;
    while (SDL_PollEvent(&event))
      processEvent(event);
    mUpdateMutex.unlock();
  }
}

bool SDLEventSystem::init()
{
  SDL_SetEventFilter(&processEventFilter, (void*)this);
  return true;
}

int SDLEventSystem::processEventFilter(void *ptr, SDL_Event *event)
{
  SDLEventSystem *system = static_cast<SDLEventSystem*>(ptr);
  system->processEvent(*event);
  return 0;
}

void SDLEventSystem::processEvent(const SDL_Event &event)
{
  switch (event.type)
  {
    case SDL_QUIT:
      processQuitEvent(event.quit);
      break;
//    case SDL_MULTIGESTURE:
//      processGestureEvent(event.mgesture);
//      break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      processKeyBoardEvent(event.key);
      break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      processMouseButtonEvent(event.button);
      break;
    case SDL_MOUSEMOTION:
      processMouseMotionEvent(event.motion);
      break;
    case SDL_MOUSEWHEEL:
      processMouseWheelEvent(event.wheel);
      break;
    case SDL_FINGERMOTION:
    case SDL_FINGERDOWN:
    case SDL_FINGERUP:
      processTouchFingerEvent(event.tfinger);
      break;
    case SDL_WINDOWEVENT:
      processWindowEvent(event.window);
      break;
      
  }
}

void SDLEventSystem::processQuitEvent(const SDL_QuitEvent &event)
{
  notify(EVENT_APP_QUIT);
}

void SDLEventSystem::processKeyBoardEvent(const SDL_KeyboardEvent &event)
{
  Event keyboardEvent;
  keyboardEvent.setTimeStamp(event.timestamp);
  
  if (event.repeat)
    keyboardEvent.setType(EVENT_KEYBOARD_REPEAT);
  else if (event.type == SDL_KEYDOWN)
    keyboardEvent.setType(EVENT_KEYBOARD_DOWN);
  else if (event.type == SDL_KEYUP)
    keyboardEvent.setType(EVENT_KEYBOARD_UP);
  
  keyboardEvent.keyboardData().window = event.windowID;
  keyboardEvent.keyboardData().keyCode = event.keysym.sym;
  keyboardEvent.keyboardData().keyModifier = event.keysym.mod;
  keyboardEvent.keyboardData().scanCode = event.keysym.scancode;
  
  if (keyboardEvent.type())
    notify(keyboardEvent);
}

void SDLEventSystem::processMouseButtonEvent(const SDL_MouseButtonEvent &event)
{
  Event mouseEvent;
  mouseEvent.setTimeStamp(event.timestamp);
  mouseEvent.mouseData().index = event.which;
  
  switch (event.state)
  {
    case SDL_PRESSED:
      mouseEvent.setType(EVENT_MOUSE_DOWN);
      break;
    case SDL_RELEASED:
      mouseEvent.setType(EVENT_MOUSE_UP);
      break;
  }
  
  mouseEvent.mouseData().buttons = event.clicks == 2 ? MOUSE_DOUBLE_CLICK : 0;
  switch (event.button)
  {
    case SDL_BUTTON_LEFT:
      mouseEvent.mouseData().buttons |= MOUSE_BUTTON_LEFT;
      break;
    case SDL_BUTTON_RIGHT:
      mouseEvent.mouseData().buttons |= MOUSE_BUTTON_RIGHT;
      break;
    case SDL_BUTTON_MIDDLE:
      mouseEvent.mouseData().buttons |= MOUSE_BUTTON_MIDDLE;
      break;
  }
  
  mouseEvent.mouseData().location = ivec2(event.x, event.y);
  mouseEvent.mouseData().delta = ivec2(0, 0);
  
  if (mouseEvent.type() && mouseEvent.mouseData().buttons & MOUSE_BUTTON_MASK)
    notify(mouseEvent);
}

void SDLEventSystem::processMouseMotionEvent(const SDL_MouseMotionEvent &event)
{
  Event mouseEvent(EVENT_MOUSE_MOVE);
  mouseEvent.setTimeStamp(event.timestamp);
  
  mouseEvent.mouseData().index = event.which;
  mouseEvent.mouseData().window = event.windowID;
  mouseEvent.mouseData().location = ivec2(event.x, event.y);
  mouseEvent.mouseData().delta = ivec2(event.xrel, event.yrel);
  
  mouseEvent.mouseData().buttons = event.state & SDL_BUTTON_LMASK ? MOUSE_BUTTON_LEFT : 0;
  mouseEvent.mouseData().buttons |= event.state & SDL_BUTTON_RMASK ? MOUSE_BUTTON_RIGHT : 0;
  mouseEvent.mouseData().buttons |= event.state & SDL_BUTTON_MMASK ? MOUSE_BUTTON_MIDDLE : 0;
  
  notify(mouseEvent);
}

void SDLEventSystem::processMouseWheelEvent(const SDL_MouseWheelEvent &event)
{
  Event mouseEvent(EVENT_MOUSE_WHEEL);
  mouseEvent.setTimeStamp(event.timestamp);
  
  mouseEvent.mouseData().index = event.which;
  mouseEvent.mouseData().window = event.windowID;
  mouseEvent.mouseData().buttons = 0;
  mouseEvent.mouseData().location = ivec2(0, 0);
  mouseEvent.mouseData().delta = ivec2(event.x, event.y);
  
  notify(mouseEvent);
}

void SDLEventSystem::processTouchFingerEvent(const SDL_TouchFingerEvent &event)
{
  Event touchEvent;
  touchEvent.setTimeStamp(event.timestamp);
  
  switch (event.type)
  {
    case SDL_FINGERDOWN:
      touchEvent.setType(EVENT_TOUCH_DOWN);
      break;
    case SDL_FINGERUP:
      touchEvent.setType(EVENT_TOUCH_UP);
      break;
    case SDL_FINGERMOTION:
      touchEvent.setType(EVENT_TOUCH_MOVE);
      break;
  }
  
  touchEvent.touchData().index = (int)event.fingerId;
  touchEvent.touchData().device = (int)event.touchId;
  touchEvent.touchData().pressure = event.pressure;
  touchEvent.touchData().location = vec2(event.x, event.y);
  touchEvent.touchData().delta = vec2(event.dx, event.dy);
  
  if (touchEvent.type())
    notifyTouch(touchEvent);
}

void SDLEventSystem::notifyTouch(Event &event)
{
  bool dispatch = false;
  int index = event.touchData().index;
  
  SDL_AtomicLock(&mTouchLock);
  if (event == EVENT_TOUCH_UP)
  {
    mTouches.erase(index);
    resetTouches();
    mLastDistance = 0.0f;
    dispatch = true;
  }
  else
  {
    if (!mTouches.count(index))
    {
      Touch &touch = mTouches[index];
      touch.nextLocation = touch.lastLocation = event.touchData().location;
      resetTouches();
      mLastDistance = 0.0f;
      dispatch = true;
    }
    else if (mTouches.size() > 1)
    {
      Touch &touch = mTouches[index];
      touch.lastLocation = touch.nextLocation;
      touch.nextLocation = event.touchData().location;
      touch.updated = true;
      
      if (checkUpdatedTouches())
      {
        event.setType(EVENT_TOUCH_GESTURE);
        event.touchData().index = (int)mTouches.size();
        event.touchData().location = getGestureLocation();
        event.touchData().delta = vec2(getGestureTheta(), getGestureDelta());
        resetTouches();
        dispatch = true;
      }
    }
    else
      dispatch = true;
  }
  SDL_AtomicUnlock(&mTouchLock);
  
  if (dispatch)
    notify(event);
}

bool SDLEventSystem::checkUpdatedTouches()
{
  bool updated = true;
  for (map<int, Touch>::iterator itr = mTouches.begin(); itr != mTouches.end(); ++itr)
    updated &= itr->second.updated;
  return updated;
}

void SDLEventSystem::resetTouches()
{
  for (map<int, Touch>::iterator itr = mTouches.begin(); itr != mTouches.end(); ++itr)
    itr->second.updated = false;
}

vec2 SDLEventSystem::getGestureLocation()
{
  map<int, Touch>::iterator itr = mTouches.begin();
  vec2 location = itr->second.nextLocation;
  for (++itr; itr != mTouches.end(); ++itr)
    location += itr->second.nextLocation;
  return location/(float)mTouches.size();
}

float SDLEventSystem::getGestureTheta()
{
  // TODO: Implement Gesture Theta when needed.
  return 0;
}

float SDLEventSystem::getGestureDelta()
{
  float distance = 0.0f;
  float delta = 0.0f;
  for (map<int, Touch>::iterator i = mTouches.begin(); i != mTouches.end(); ++i)
  {
    map<int, Touch>::iterator j = i;
    for (++j; j != mTouches.end(); ++j)
      distance += (i->second.nextLocation - j->second.nextLocation).length();
  }
  if (mLastDistance > 0.0f)
    delta = distance - mLastDistance;
  mLastDistance = distance;
  return delta;
}

void SDLEventSystem::processGestureEvent(const SDL_MultiGestureEvent &event)
{
  Event touchEvent(EVENT_TOUCH_GESTURE);
  touchEvent.setTimeStamp(event.timestamp);
  
  touchEvent.touchData().index = (int)event.numFingers;
  touchEvent.touchData().device = (int)event.touchId;
  touchEvent.touchData().pressure = 0.0f;
  touchEvent.touchData().location = vec2(event.x, event.y);
  touchEvent.touchData().delta = vec2(event.dTheta, event.dDist);
  
  notify(touchEvent);
}

void SDLEventSystem::processWindowEvent(const SDL_WindowEvent &event)
{
  Event windowEvent(EVENT_WINDOW);
  windowEvent.setTimeStamp(event.timestamp);
  windowEvent.windowData().event = EVENT_WINDOW_NONE;
  windowEvent.windowData().window = event.windowID;
  windowEvent.windowData().data = ivec2(event.data1, event.data2);
  
  switch (event.event)
  {
    case SDL_WINDOWEVENT_SHOWN:
      windowEvent.windowData().event = EVENT_WINDOW_SHOWN;
      break;
    case SDL_WINDOWEVENT_HIDDEN:
      windowEvent.windowData().event = EVENT_WINDOW_HIDE;
      break;
    case SDL_WINDOWEVENT_EXPOSED:
      windowEvent.windowData().event = EVENT_WINDOW_EXPOSED;
      break;
    case SDL_WINDOWEVENT_MOVED:
      windowEvent.windowData().event = EVENT_WINDOW_MOVED;
      break;
    case SDL_WINDOWEVENT_RESIZED:
    //case SDL_WINDOWEVENT_SIZE_CHANGED:
      windowEvent.windowData().event = EVENT_WINDOW_RESIZED;
      break;
    case SDL_WINDOWEVENT_MINIMIZED:
      windowEvent.windowData().event = EVENT_WINDOW_MINIMIZED;
      break;
    case SDL_WINDOWEVENT_MAXIMIZED:
      windowEvent.windowData().event = EVENT_WINDOW_MAXIMIZED;
      break;
    case SDL_WINDOWEVENT_RESTORED:
      windowEvent.windowData().event = EVENT_WINDOW_RESTORED;
      break;
    case SDL_WINDOWEVENT_ENTER:
      windowEvent.windowData().event = EVENT_WINDOW_ENTER;
      break;
    case SDL_WINDOWEVENT_LEAVE:
      windowEvent.windowData().event = EVENT_WINDOW_LEAVE;
      break;
    case SDL_WINDOWEVENT_CLOSE:
      windowEvent.windowData().event = EVENT_WINDOW_CLOSE;
      break;
  }
  
  if (windowEvent.windowData().event != EVENT_WINDOW_NONE)
    notify(windowEvent);
}
