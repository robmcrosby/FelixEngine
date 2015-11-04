//
//  SDLEventSystem.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 11/3/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "SDLEventSystem.h"
#include "FelixEngine.h"

using namespace fx;
using namespace std;

SDLEventSystem::SDLEventSystem(): mEngine(FelixEngine::Instance())
{
  setEventFlags(EVENT_UPDATE);
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
  return true;
}

void SDLEventSystem::processEvent(const SDL_Event &event)
{
  switch (event.type)
  {
    case SDL_QUIT:
      processQuitEvent(event.quit);
      break;
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
    case SDL_MULTIGESTURE:
      processGestureEvent(event.mgesture);
      break;
    case SDL_WINDOWEVENT:
      processWindowEvent(event.window);
      break;
      
  }
}

void SDLEventSystem::processQuitEvent(const SDL_QuitEvent &event)
{
  //cout << "Process Quit Event" << endl;
  mEngine->exit();
}

void SDLEventSystem::processKeyBoardEvent(const SDL_KeyboardEvent &event)
{
  Event keyboardEvent;
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
  
  if (mEngine && keyboardEvent.type())
    mEngine->notify(keyboardEvent);
}

void SDLEventSystem::processMouseButtonEvent(const SDL_MouseButtonEvent &event)
{
  //cout << "Process Mouse Button Event" << endl;
}

void SDLEventSystem::processMouseMotionEvent(const SDL_MouseMotionEvent &event)
{
  //cout << "Process Mouse Motion Event" << endl;
}

void SDLEventSystem::processMouseWheelEvent(const SDL_MouseWheelEvent &event)
{
  //cout << "Process Mouse Wheel Event" << endl;
}

void SDLEventSystem::processTouchFingerEvent(const SDL_TouchFingerEvent &event)
{
  Event touchEvent;
  
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
  
  if (mEngine && touchEvent.type())
    mEngine->notify(touchEvent);
}

void SDLEventSystem::processGestureEvent(const SDL_MultiGestureEvent &event)
{
  //cout << "Process MultiGesture Event" << endl;
}

void SDLEventSystem::processWindowEvent(const SDL_WindowEvent &event)
{
  Event windowEvent;
  windowEvent.windowData().window = event.windowID;
  
  switch (event.event)
  {
    case SDL_WINDOWEVENT_SHOWN:
      windowEvent.setType(EVENT_WINDOW_SHOWN);
      break;
    case SDL_WINDOWEVENT_HIDDEN:
      windowEvent.setType(EVENT_WINDOW_HIDE);
      break;
    case SDL_WINDOWEVENT_EXPOSED:
      windowEvent.setType(EVENT_WINDOW_EXPOSED);
      break;
    case SDL_WINDOWEVENT_MOVED:
      windowEvent.setType(EVENT_WINDOW_MOVED);
      windowEvent.windowData().data = ivec2(event.data1, event.data2);
      break;
    case SDL_WINDOWEVENT_RESIZED:
    case SDL_WINDOWEVENT_SIZE_CHANGED:
      windowEvent.setType(EVENT_WINDOW_RESIZED);
      windowEvent.windowData().data = ivec2(event.data1, event.data2);
      break;
    case SDL_WINDOWEVENT_MINIMIZED:
      windowEvent.setType(EVENT_WINDOW_MINIMIZED);
      break;
    case SDL_WINDOWEVENT_MAXIMIZED:
      windowEvent.setType(EVENT_WINDOW_MAXIMIZED);
      break;
    case SDL_WINDOWEVENT_RESTORED:
      windowEvent.setType(EVENT_WINDOW_RESTORED);
      break;
    case SDL_WINDOWEVENT_ENTER:
      windowEvent.setType(EVENT_WINDOW_ENTER);
      break;
    case SDL_WINDOWEVENT_LEAVE:
      windowEvent.setType(EVENT_WINDOW_LEAVE);
      break;
    case SDL_WINDOWEVENT_CLOSE:
      windowEvent.setType(EVENT_WINDOW_CLOSE);
      break;
  }
  
  if (mEngine && windowEvent.type())
    mEngine->notify(windowEvent);
}
