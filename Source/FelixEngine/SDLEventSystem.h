//
//  SDLEventSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 11/3/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef SDLEventSystem_h
#define SDLEventSystem_h

#include "EventSystem.h"


namespace fx
{
  class SDLEventSystem: public EventSystem
  {
  public:
    SDLEventSystem();
    virtual ~SDLEventSystem();
    
    virtual void pollEvents();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
    
  private:
    static int processEventFilter(void *ptr, SDL_Event *event);
    void processEvent(const SDL_Event &event);
    
    void processQuitEvent(const SDL_QuitEvent &event);
    void processKeyBoardEvent(const SDL_KeyboardEvent &event);
    
    void processMouseButtonEvent(const SDL_MouseButtonEvent &event);
    void processMouseMotionEvent(const SDL_MouseMotionEvent &event);
    void processMouseWheelEvent(const SDL_MouseWheelEvent &event);
    
    void processTouchFingerEvent(const SDL_TouchFingerEvent &event);
    void processGestureEvent(const SDL_MultiGestureEvent &event);
    
    void processWindowEvent(const SDL_WindowEvent &event);
    
  private:
    void notifyTouch(Event &event);
    bool checkUpdatedTouches();
    void resetTouches();
    
    vec2 getGestureLocation();
    float getGestureTheta();
    float getGestureDelta();
    
    struct Touch
    {
      vec2 nextLocation;
      vec2 lastLocation;
      bool updated;
    };
    
    Mutex mUpdateMutex;
    
    SDL_SpinLock mTouchLock;
    std::map<int, Touch> mTouches;
    float mLastDistance;
  };
}

#endif /* SDLEventSystem_h */
