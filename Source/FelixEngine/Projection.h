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
    /**
     *
     */
    Projection(Scene *scene): Component(scene),
      mType(PROJ_ORTHO),
      mAspect(ASPECT_NONE),
      mRenderSlots(0),
      mLock(0),
      mDisparity(0),
      mZeroDistance(1.0f)
    {
    }
    
    /**
     *
     */
    virtual ~Projection() {}
    
    virtual void setToXml(const XMLTree::Node &node)
    {
      Component::setToXml(node);
      if (node.hasAttribute("type"))
        setType(node.attribute("type"));
      if (node.hasAttribute("aspect"))
        setAspect(node.attribute("aspect"));
      if (node.hasAttribute("disparity"))
        setDisparity(node.attributeAsFloat("disparity"));
      if (node.hasAttribute("zero"))
        setZeroDistance(node.attributeAsFloat("zero"));
      if (node.hasSubNode("Volume"))
        mVolume.setToXml(node.subNode("Volume"));
    }
    
    /**
     *
     */
    virtual bool init()
    {
      mRenderSlots = getSibling<RenderSlots>();
      return Component::init() && mRenderSlots;
    }
    
    /**
     *
     */
    void setVolume(const Volume &volume)
    {
      lock();
      mVolume = volume;
      unlock();
    }
    
    /**
     *
     */
    Volume volume() const
    {
      lock();
      Volume ret = mVolume;
      unlock();
      return ret;
    }
    
    /**
     *
     */
    void setType(const std::string &str) {setType(ParseProjectionType(str));}
    
    /**
     *
     */
    void setType(PROJ_TYPE type)
    {
      lock();
      mType = type;
      unlock();
    }
    
    /**
     *
     */
    PROJ_TYPE type() const
    {
      lock();
      PROJ_TYPE ret = mType;
      unlock();
      return ret;
    }
    
    /**
     *
     */
    void setAspect(const std::string &str) {setAspect(ParseAspectType(str));}
    
    /**
     *
     */
    void setAspect(ASPECT_TYPE aspect)
    {
      lock();
      mAspect = aspect;
      unlock();
    }
    
    /**
     *
     */
    ASPECT_TYPE aspect() const
    {
      lock();
      ASPECT_TYPE ret = mAspect;
      unlock();
      return ret;
    }
    
    /**
     *
     */
    void setDisparity(float disp)
    {
      lock();
      mDisparity = disp;
      unlock();
    }
    
    /**
     *
     */
    float disparity() const
    {
      lock();
      float ret = mDisparity;
      unlock();
      return ret;
    }
    
    /**
     *
     */
    void setZeroDistance(float dist)
    {
      lock();
      mZeroDistance = dist;
      unlock();
    }
    
    /**
     *
     */
    float zeroDistance() const
    {
      lock();
      float ret = mZeroDistance;
      unlock();
      return ret;
    }
    
    /**
     *
     */
    void lock() const {SDL_AtomicLock(&mLock);}
    
    /**
     *
     */
    void unlock() const {SDL_AtomicUnlock(&mLock);}
    
    /**
     *
     */
    mat4 getProjection(vec2 size, int flags) const
    {
      lock();
      Volume v = mVolume;
      ASPECT_TYPE aspect = mAspect;
      PROJ_TYPE type = mType;
      float disparity = mDisparity/2.0f;
      float zeroDistance = mZeroDistance;
      unlock();
      
      applyDisparity(v, disparity, zeroDistance, flags);
      applyAspect(v, size, aspect);
      
      mat4 modification = getModification(flags);
      
      if (type == PROJ_ORTHO)
        return modification * mat4::Ortho(v.left, v.right, v.bottom, v.top, v.near, v.far);
      return modification * mat4::Frustum(v.left, v.right, v.bottom, v.top, v.near, v.far) * mat4::Trans3d(vec3(disparity, 0.0f, 0.0f));
    }
    
  protected:
    /**
     *
     */
    virtual void update()
    {
      if (mRenderSlots)
      {
        for (RenderSlot *slot : *mRenderSlots)
        {
          if (slot)
            applyToSlot(*slot);
        }
      }
      Component::update();
    }
    
  private:
    /**
     *
     */
    void applyToSlot(RenderSlot &slot) const
    {
      // First apply a screen split if set in the projection flags.
      int flags = slot.projectionFlags();
      Frame *frame = GetResource<Frame>(&slot.targets());
      if (frame && flags & PROJ_SPLIT)
        applySplit(slot, frame->size());
      
      applyProjection(slot);
    }
    
    /**
     *
     */
    void applySplit(RenderSlot &slot, ivec2 size) const
    {
      int flags = slot.projectionFlags();
      int rotation = flags & PROJ_ROTATION;
      ivec2 position;
      
      if (rotation == PROJ_ROT_CW || rotation == PROJ_ROT_CCW)
      {
        size.y /= 2;
        if ((flags & PROJ_LEFT  && rotation == PROJ_ROT_CW) ||
            (flags & PROJ_RIGHT && rotation == PROJ_ROT_CCW))
          position.y = size.y;
      }
      else
      {
        size.x /= 2;
        if ((flags & PROJ_RIGHT && rotation == PROJ_ROT_NONE) ||
            (flags & PROJ_LEFT && rotation == PROJ_ROT_FULL))
          position.x = size.x;
      }
      
      slot.drawState().viewport.scissor = true;
      slot.drawState().viewport.size = size;
      slot.drawState().viewport.position = position;
    }
    
    /**
     *
     */
    void applyProjection(RenderSlot &slot) const
    {
      vec2 size(slot.drawState().viewport.size);
      if (size == vec2())
      {
        Frame *frame = GetResource<Frame>(&slot.targets());
        size = frame ? vec2(frame->size()) : vec2(1.0f, 1.0f);
      }
      
      int flags = slot.projectionFlags();
      int rotation = flags & PROJ_ROTATION;
      if (rotation == PROJ_ROT_CW || rotation == PROJ_ROT_CCW)
        size = size.yx();
      
      mat4 projection = getProjection(size, flags);
      slot.setUniform("Projection", projection);
    }
    
    /**
     *
     */
    mat4 getModification(int flags) const
    {
      int rotation = flags & PROJ_ROTATION;
      if (rotation == PROJ_ROT_CW)
        return mat4::RotZ(-90.0f * fx::DegToRad);
      if (rotation == PROJ_ROT_CCW)
        return mat4::RotZ(90.0f * fx::DegToRad);
      if (rotation == PROJ_ROT_FULL)
        return mat4::RotZ(180.0f * fx::DegToRad);
      return mat4();
    }
    
    /**
     *
     */
    void applyDisparity(Volume &volume, float &disparity, float zeroDistance, int flags) const
    {
      int stereo = flags & PROJ_STEREO;
      if (stereo == PROJ_MONO)
      {
        // No Disparity for Mono
        disparity = 0.0f;
      }
      else
      {
        float shift = disparity * (volume.near/zeroDistance);
        if (stereo == PROJ_LEFT)
        {
          // Adjust for the Left Side
          volume.left += shift;
          volume.right += shift;
        }
        else
        {
          // Adjust for the Right Side
          volume.left -= shift;
          volume.right -= shift;
          disparity = -disparity;
        }
      }
    }
    
    /**
     *
     */
    void applyAspect(Volume &volume, vec2 size, ASPECT_TYPE aspect) const
    {
      if (aspect == ASPECT_WIDTH || (aspect == ASPECT_AUTO && size.w < size.h))
      {
        float aspectRatio = size.h/size.w;
        volume.top *= aspectRatio;
        volume.bottom *= aspectRatio;
      }
      else if (aspect == ASPECT_HEIGHT || (aspect == ASPECT_AUTO && size.h < size.w))
      {
        float aspectRatio = size.w/size.h;
        volume.left *= aspectRatio;
        volume.right *= aspectRatio;
      }
    }
    
    PROJ_TYPE   mType;   /**<  */
    ASPECT_TYPE mAspect; /**<  */
    Volume      mVolume; /**<  */
    
    float mDisparity;    /**<  */
    float mZeroDistance; /**<  */
    
    mutable SDL_SpinLock mLock; /**<  */
    RenderSlots *mRenderSlots;  /**<  */
  };
}

#endif /* Projection_h */
