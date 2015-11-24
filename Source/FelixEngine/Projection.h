//
//  Projection.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Projection_h
#define Projection_h

#include "Component.h"
#include "Matrix.h"

namespace fx
{
  class Frame;
  class RenderSlots;
  
  /**
   *
   */
  enum PROJ_TYPE
  {
    PROJ_ORTHO,
    PROJ_FRUSTUM,
  };
  
  /**
   *
   */
  enum ASPECT_TYPE
  {
    ASPECT_AUTO,
    ASPECT_WIDTH,
    ASPECT_HEIGHT,
    ASPECT_NONE,
  };
  
  /**
   *
   */
  struct Volume
  {
    Volume(): near(0), far(1), left(-1), right(1), top(1), bottom(1) {}
    bool setToXml(const XMLTree::Node *node);
    float width() const {return right - left;}
    float height() const {return top - bottom;}
    
    float near, far;
    float left, right;
    float top, bottom;
  };
  
  /**
   *
   */
  class Projection: public Component
  {
  public:
    PROJ_TYPE GetProjectionType(const std::string &str);
    ASPECT_TYPE GetAspectType(const std::string &str);
    
  public:
    Projection(Object *obj);
    virtual ~Projection();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
    
    void setVolume(const Volume &volume)
    {
      lock();
      mVolume = volume;
      unlock();
    }
    Volume volume() const
    {
      lock();
      Volume ret = mVolume;
      unlock();
      return ret;
    }
    
    void setType(const std::string &str);
    void setType(PROJ_TYPE type)
    {
      lock();
      mType = type;
      unlock();
    }
    PROJ_TYPE type() const
    {
      lock();
      PROJ_TYPE ret = mType;
      unlock();
      return ret;
    }
    
    void setAspect(const std::string &str);
    void setAspect(ASPECT_TYPE aspect)
    {
      lock();
      mAspect = aspect;
      unlock();
    }
    ASPECT_TYPE aspect() const
    {
      lock();
      ASPECT_TYPE ret = mAspect;
      unlock();
      return ret;
    }
    
    void lock() const {SDL_AtomicLock(&mLock);}
    void unlock() const {SDL_AtomicUnlock(&mLock);}
    
  private:
    void update(void*);
    mat4 toMatrix4x4(vec2 size) const;
    
    PROJ_TYPE   mType;
    ASPECT_TYPE mAspect;
    Volume      mVolume;
    
    mutable SDL_SpinLock mLock;
    RenderSlots *mRenderSlots;
  };
}

#endif /* Projection_h */
