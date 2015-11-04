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
    
    Mutex mUpdateMutex;
  };
}

#endif /* SDLEventSystem_h */
