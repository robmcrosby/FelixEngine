//
//  Event.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/28/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Event_h
#define Event_h

#include "Platform.h"
#include "Pool.h"
#include "Vector.h"

#define DATA_SIZE 32


namespace fx
{
  class EventHandler;
  
  /**
   *
   */
  enum EVENT_TYPE
  {
    EVENT_NONE    = 0x00, /**< No event */
    EVENT_RENDER  = 0x01, /**< Render */
    EVENT_UPDATE  = 0x02, /**< Update */
    
    EVENT_KEYBOARD        = 0x70, /**< Keyboard Event Mask */
    EVENT_KEYBOARD_DOWN   = 0x10, /**< Key has been pressed */
    EVENT_KEYBOARD_UP     = 0x20, /**< Key has been released */
    EVENT_KEYBOARD_REPEAT = 0x40, /**< Key has been held down */
    
    EVENT_TOUCH      = 0x0380, /**< Touch Event Mask */
    EVENT_TOUCH_DOWN = 0x0080, /**< Touch has made contact */
    EVENT_TOUCH_UP   = 0x0100, /**< Touch has left contact */
    EVENT_TOUCH_MOVE = 0x0200, /**< Touch have moved */
    
    EVENT_MOUSE       = 0x3C00, /**< Mouse Event Mask */
    EVENT_MOUSE_DOWN  = 0x0400, /**< Mouse Button has been pressed */
    EVENT_MOUSE_UP    = 0x0800, /**< Mouse Button has been released */
    EVENT_MOUSE_MOVE  = 0x1000, /**< Mouse has been moved */
    EVENT_MOUSE_WHEEL = 0x2000, /**< Mouse Wheel has been moved */
    
    EVENT_WINDOW = 0x3FF00, /**< Window event mask */
    
    EVENT_WINDOW_SHOWN   = 0x0100, /**< Window has been shown */
    EVENT_WINDOW_HIDE    = 0x0200, /**< Window has been hidden */
    EVENT_WINDOW_EXPOSED = 0x0400, /**< Window has been exposed and should be redrawn */
    EVENT_WINDOW_MOVED   = 0x0800, /**< Window has been moved */
    
    EVENT_WINDOW_RESIZED   = 0x1000, /**< Window has been resized */
    EVENT_WINDOW_MINIMIZED = 0x2000, /**< Window has been minimized */
    EVENT_WINDOW_MAXIMIZED = 0x3000, /**< Window has been maximized */
    EVENT_WINDOW_RESTORED  = 0x4000, /**< Window has been restored to normal size and position */
    
    EVENT_WINDOW_ENTER = 0x08000, /**< Window has gained mouse focus */
    EVENT_WINDOW_LEAVE = 0x10000, /**< Window has lost mouse focus */
    EVENT_WINDOW_CLOSE = 0x20000, /**< Window has been closed */
    
    EVENT_OTHER   = 0xFF000000,
    EVENT_ALL     = 0xFFFFFFFF,
  };
  
  enum DISPATCH_TYPE
  {
    DISPATCH_SERIAL,   /**< Notify the listeners with out tasking */
    DISPATCH_SINGLE,   /**< Notify the listeners with a single task */
    DISPATCH_MULTIPLE, /**< Notify each listener with their own task */
    DISPATCH_BLOCK,    /**< Notify and wait for the tasks to complete */
  };
  
  class Event
  {
  public:
    Event(EVENT_TYPE type = EVENT_NONE, DISPATCH_TYPE dispatch = DISPATCH_SINGLE): mType(type), mDispatch(dispatch), mSender(0), mTarget(0) {setTimeStamp();}
    Event(const Event &other) {*this = other;}
    
    Event& operator=(const Event &other)
    {
      mType = other.mType;
      mDispatch = other.mDispatch;
      mTimeStamp = other.mTimeStamp;
      mSender = other.mSender;
      mTarget = other.mTarget;
      memcpy(mData, other.mData, DATA_SIZE);
      return *this;
    }
    
    bool operator==(EVENT_TYPE type) const {return mType == type;}
    
    void setType(EVENT_TYPE type) {mType = type;}
    EVENT_TYPE type() const {return mType;}
    
    void setDispatchType(DISPATCH_TYPE dispatch) {mDispatch = dispatch;}
    DISPATCH_TYPE dispatchType() const {return mDispatch;}
    
    void setTimeStamp() {mTimeStamp = Platform::GetTimeStamp();}
    void setTimeStamp(unsigned int ts) {mTimeStamp = ts;}
    unsigned int timeStamp() const {return mTimeStamp;}
    
    void setSender(EventHandler *sender) {mSender = sender;}
    EventHandler* sender() const {return mSender;}
    
    void setTarget(EventHandler *target) {mTarget = target;}
    EventHandler* target() const {return mTarget;}
    
    static Pool<Event>& EventPool()
    {
      static Pool<Event> pool;
      return pool;
    }
    Event* copy() const
    {
      Event *event = EventPool().newItem();
      *event = *this;
      return event;
    }
    
  public:
    struct WindowData
    {
      int window;
      ivec2 data;
    };
    WindowData& windowData() {return *(WindowData*)mData;}
    const WindowData& windowData() const {return *(WindowData*)mData;}
    
    struct KeyboardData
    {
      int window, keyCode, keyModifier, scanCode;
    };
    KeyboardData& keyboardData() {return *(KeyboardData*)mData;}
    const KeyboardData& keyboardData() const {return *(KeyboardData*)mData;}
    
    struct TouchData
    {
      int index, device;
      float pressure;
      vec2 location;
      vec2 delta;
    };
    TouchData& touchData() {return *(TouchData*)mData;}
    const TouchData& touchData() const {return *(TouchData*)mData;}
    
  private:
    EVENT_TYPE    mType;
    DISPATCH_TYPE mDispatch;
    unsigned int  mTimeStamp;
    
    EventHandler *mSender;
    EventHandler *mTarget;
    
    char mData[DATA_SIZE];
  };
}


#endif /* Event_h */
