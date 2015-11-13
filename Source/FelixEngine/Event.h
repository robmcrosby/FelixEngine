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

#define DATA_SIZE 64


namespace fx
{
  class EventHandler;
  
  /**
   *
   */
  enum EVENT_TYPE
  {
    EVENT_ALL              = 0xFFFFFFFF, /** All Events */
    EVENT_NONE             = 0x00000000, /**< No Events */
    
    EVENT_APP_MASK         = 0x0000000F, /**< Application events Mask */
    EVENT_APP_RENDER       = 0x00000001, /**< Application render */
    EVENT_APP_UPDATE       = 0x00000002, /**< Application update */
    EVENT_APP_QUIT         = 0x00000004, /**< Application quit */
    
    EVENT_TOUCH_MASK       = 0x000000F0, /**< Touch events Mask */
    EVENT_TOUCH_DOWN       = 0x00000010, /**< Touch has made contact */
    EVENT_TOUCH_UP         = 0x00000020, /**< Touch has left contact */
    EVENT_TOUCH_MOVE       = 0x00000040, /**< Touch have moved */
    EVENT_TOUCH_GESTURE    = 0x00000080, /**< Touch gesture made */
    
    EVENT_MOUSE_MASK       = 0x00000F00, /**< Mouse events Mask */
    EVENT_MOUSE_DOWN       = 0x00000100, /**< Mouse button has been pressed */
    EVENT_MOUSE_UP         = 0x00000200, /**< Mouse button has been released */
    EVENT_MOUSE_MOVE       = 0x00000400, /**< Mouse has been moved */
    EVENT_MOUSE_WHEEL      = 0x00000800, /**< Mouse wheel has been moved */
    
    EVENT_KEYBOARD_MASK    = 0x00007000, /**< Keyboard events Mask */
    EVENT_KEYBOARD_DOWN    = 0x00001000, /**< Key has been pressed */
    EVENT_KEYBOARD_UP      = 0x00002000, /**< Key has been released */
    EVENT_KEYBOARD_REPEAT  = 0x00004000, /**< Key has been held down */
    
    EVENT_WINDOW_MASK      = 0x03FF0000, /**< Window events mask */
    EVENT_WINDOW_SHOWN     = 0x00010000, /**< Window has been shown */
    EVENT_WINDOW_HIDE      = 0x00020000, /**< Window has been hidden */
    EVENT_WINDOW_EXPOSED   = 0x00040000, /**< Window has been exposed and should be redrawn */
    EVENT_WINDOW_MOVED     = 0x00080000, /**< Window has been moved */
    EVENT_WINDOW_RESIZED   = 0x00100000, /**< Window has been resized */
    EVENT_WINDOW_MINIMIZED = 0x00200000, /**< Window has been minimized */
    EVENT_WINDOW_MAXIMIZED = 0x00300000, /**< Window has been maximized */
    EVENT_WINDOW_RESTORED  = 0x00400000, /**< Window has been restored to normal size and position */
    EVENT_WINDOW_ENTER     = 0x00800000, /**< Window has gained mouse focus */
    EVENT_WINDOW_LEAVE     = 0x01000000, /**< Window has lost mouse focus */
    EVENT_WINDOW_CLOSE     = 0x02000000, /**< Window has been closed */
    
    EVENT_MOTION_MASK      = 0x0C000000, /**< Motion events mask */
    EVENT_MOTION_ROTATION  = 0x04000000, /**< Device has been rotated */
    EVENT_MOTION_MOVEMENT  = 0x08000000, /**< Device has been moved */
    EVENT_MOTION_COMBINED  = 0x0C000000, /**< Device movement combined */
    
    EVENT_USER_MASK        = 0xF0000000, /**< User events mask */
    EVENT_USER_1           = 0x10000000, /**< User event 1 */
    EVENT_USER_2           = 0x20000000, /**< User event 2 */
    EVENT_USER_3           = 0x40000000, /**< User event 3 */
    EVENT_USER_4           = 0x80000000, /**< User event 4 */
  };
  
  enum DISPATCH_TYPE
  {
    DISPATCH_SERIAL,   /**< Notify the listeners with out tasking */
    DISPATCH_SINGLE,   /**< Notify the listeners with a single task */
    DISPATCH_MULTIPLE, /**< Notify each listener with their own task */
    DISPATCH_BLOCK,    /**< Notify and wait for the tasks to complete */
  };
  
  enum MOUSE_BUTTON
  {
    MOUSE_BUTTON_MASK   = 0x0f,
    MOUSE_BUTTON_LEFT   = 0x01,
    MOUSE_BUTTON_RIGHT  = 0x02,
    MOUSE_BUTTON_MIDDLE = 0x04,
    MOUSE_DOUBLE_CLICK  = 0x10,
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
      int window;
      int keyCode;
      int keyModifier;
      int scanCode;
    };
    KeyboardData& keyboardData() {return *(KeyboardData*)mData;}
    const KeyboardData& keyboardData() const {return *(KeyboardData*)mData;}
    
    struct TouchData
    {
      int index;
      int device;
      float pressure;
      vec2 location;
      vec2 delta;
    };
    TouchData& touchData() {return *(TouchData*)mData;}
    const TouchData& touchData() const {return *(TouchData*)mData;}
    
    struct MouseData
    {
      int index;
      int window;
      int buttons;
      ivec2 location;
      ivec2 delta;
    };
    MouseData& mouseData() {return *(MouseData*)mData;}
    const MouseData& mouseData() const {return *(MouseData*)mData;}
    
    struct MotionData
    {
      vec3 acceleration;
      vec3 rotationRate;
      vec3 gravity;
    };
    MotionData& motionData() {return *(MotionData*)mData;}
    const MotionData& motionData() const {return *(MotionData*)mData;}
    
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
