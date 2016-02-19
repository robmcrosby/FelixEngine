//
//  Projection.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Projection_h
#define Projection_h

#include "RenderSlots.h"
#include "Matrix.h"

namespace fx
{
  class Frame;
  
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
  enum PROJ_MODE
  {
    PROJ_NORMAL,
    PROJ_INVERT_X,
    PROJ_INVERT_Y,
    PROJ_ROTATE_CW,
    PROJ_ROTATE_CCW,
    PROJ_ROTATE_FULL,
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
    void setToXml(const XMLTree::Node *node)
    {
      near   = node->attributeAsFloat("near");
      far    = node->attributeAsFloat("far");
      left   = node->attributeAsFloat("left");
      right  = node->attributeAsFloat("right");
      top    = node->attributeAsFloat("top");
      bottom = node->attributeAsFloat("bottom");
    }
    float width() const {return right - left;}
    float height() const {return top - bottom;}
    
    float near;
    float far;
    float left;
    float right;
    float top;
    float bottom;
  };
  
  /**
   *
   */
  class Projection: public Component
  {
  public:
    /**
     *
     */
    static PROJ_TYPE ParseProjectionType(const std::string &str)
    {
      if (str == "ortho")
        return PROJ_ORTHO;
      return PROJ_FRUSTUM;
    }
    
    /**
     *
     */
    static PROJ_MODE ParseProjectionMode(const std::string &str)
    {
      if (str == "invert_x")
        return PROJ_INVERT_X;
      if (str == "invert_y")
        return PROJ_INVERT_Y;
      if (str == "rotate_cw")
        return PROJ_ROTATE_CW;
      if (str == "rotate_ccw")
        return PROJ_ROTATE_CCW;
      if (str == "rotate_full")
        return PROJ_ROTATE_FULL;
      return PROJ_NORMAL;
    }
    
    /**
     *
     */
    static ASPECT_TYPE ParseAspectType(const std::string &str)
    {
      if (str == "auto")
        return ASPECT_AUTO;
      if (str == "width")
        return ASPECT_WIDTH;
      if (str == "height")
        return ASPECT_HEIGHT;
      return ASPECT_NONE;
    }
    
  public:
    Projection(Scene *scene): Component(scene), mType(PROJ_ORTHO),
    mAspect(ASPECT_NONE), mRenderSlots(0), mLock(0), mDisparity(0), mZeroDistance(1.0f) {}
    virtual ~Projection() {}
    
    virtual void setToXml(const XMLTree::Node &node)
    {
      Component::setToXml(node);
      if (node.hasAttribute("type"))
        setType(node.attribute("type"));
      if (node.hasAttribute("mode"))
        setMode(node.attribute("mode"));
      if (node.hasAttribute("aspect"))
        setAspect(node.attribute("aspect"));
      if (node.hasAttribute("disparity"))
        setDisparity(node.attributeAsFloat("disparity"));
      if (node.hasAttribute("zero"))
        setZeroDistance(node.attributeAsFloat("zero"));
      if (node.hasSubNode("Volume"))
        mVolume.setToXml(node.subNode("Volume"));
    }
    
    virtual bool init()
    {
      mRenderSlots = getSibling<RenderSlots>();
      return Component::init() && mRenderSlots;
    }
    
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
    
    void setType(const std::string &str) {setType(ParseProjectionType(str));}
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
    
    void setMode(const std::string &str) {setMode(ParseProjectionMode(str));}
    void setMode(PROJ_MODE mode)
    {
      lock();
      mMode = mode;
      unlock();
    }
    PROJ_MODE mode() const
    {
      lock();
      PROJ_MODE ret = mMode;
      unlock();
      return ret;
    }
    
    void setAspect(const std::string &str) {setAspect(ParseAspectType(str));}
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
    
    void setDisparity(float disp)
    {
      lock();
      mDisparity = disp;
      unlock();
    }
    float disparity() const
    {
      lock();
      float ret = mDisparity;
      unlock();
      return ret;
    }
    
    void setZeroDistance(float dist)
    {
      lock();
      mZeroDistance = dist;
      unlock();
    }
    float zeroDistance() const
    {
      lock();
      float ret = mZeroDistance;
      unlock();
      return ret;
    }
    
    void lock() const {SDL_AtomicLock(&mLock);}
    void unlock() const {SDL_AtomicUnlock(&mLock);}
    
  protected:
    virtual void update()
    {
      if (mRenderSlots)
      {
        for (RenderSlot *slot : *mRenderSlots)
        {
          vec2 size = slot->drawState().viewport.size;
          if (size == vec2())
          {
            Frame *frame = GetResource<Frame>(&slot->targets());
            size = frame ? vec2(frame->size()) : vec2(1.0f, 1.0f);
          }
          slot->setUniform("Projection", toMatrix4x4(size, slot->stereoType()));
        }
      }
      Component::update();
    }
    
  private:
    mat4 toMatrix4x4(vec2 size, STEREO_TYPE stereo) const
    {
      lock();
      Volume v = mVolume;
      ASPECT_TYPE aspect = mAspect;
      PROJ_TYPE type = mType;
      PROJ_MODE mode = mMode;
      float disparity = mDisparity/2.0f;
      float zeroDistance = mZeroDistance;
      unlock();
      
      if (mode == PROJ_ROTATE_CW || mode == PROJ_ROTATE_CCW)
        size = size.yx();
      
      if ((stereo == STEREO_LEFT || stereo == STEREO_RIGHT) && type == PROJ_FRUSTUM)
      {
        float shift = disparity * (v.near/zeroDistance);
        if (stereo == STEREO_LEFT)
        {
          v.left += shift;
          v.right += shift;
        }
        else
        {
          v.left -= shift;
          v.right -= shift;
          disparity = -disparity;
        }
      }
      else
        disparity = 0.0f;
      
      if (aspect == ASPECT_WIDTH || (aspect == ASPECT_AUTO && size.w < size.h))
      {
        float aspectRatio = size.h/size.w;
        v.top *= aspectRatio;
        v.bottom *= aspectRatio;
      }
      else if (aspect == ASPECT_HEIGHT || (aspect == ASPECT_AUTO && size.h < size.w))
      {
        float aspectRatio = size.w/size.h;
        v.left *= aspectRatio;
        v.right *= aspectRatio;
      }
      
      mat4 modeMtx;
      if (mode == PROJ_ROTATE_CW)
        modeMtx = mat4::RotZ(90*fx::DegToRad);
      if (mode == PROJ_ROTATE_CCW)
        modeMtx = mat4::RotZ(-90*fx::DegToRad);
      
      if (type == PROJ_ORTHO)
        return modeMtx * mat4::Ortho(v.left, v.right, v.bottom, v.top, v.near, v.far);
      return modeMtx * mat4::Frustum(v.left, v.right, v.bottom, v.top, v.near, v.far) * mat4::Trans3d(vec3(disparity, 0.0f, 0.0f));
    }
    
    PROJ_TYPE   mType;
    PROJ_MODE   mMode;
    ASPECT_TYPE mAspect;
    Volume      mVolume;
    
    float mDisparity;
    float mZeroDistance;
    
    mutable SDL_SpinLock mLock;
    RenderSlots *mRenderSlots;
  };
}

#endif /* Projection_h */
