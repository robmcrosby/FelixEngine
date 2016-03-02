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
   * Enum for the diffrent types of projections.
   */
  enum PROJ_TYPE
  {
    PROJ_ORTHO,   /**< Orthographic or isometric projection */
    PROJ_FRUSTUM, /**< Frustum or perspective projection */
  };
  
  /**
   * Enum for the Aspect type on how the aspect ratio effects the Projection.
   */
  enum ASPECT_TYPE
  {
    ASPECT_AUTO,   /**< Adjust to the smallest dimension. */
    ASPECT_WIDTH,  /**< Adjust to the width. */
    ASPECT_HEIGHT, /**< Adjust to the height. */
    ASPECT_NONE,   /**< Do not adjust the Projection. */
  };
  
  /**
   * Struct of parameters for a Volume used by a projection.
   */
  struct Volume
  {
    /**
     * Constructor initalizes the parameter values to a unit cube.
     */
    Volume(): near(0), far(1), left(-1), right(1), top(1), bottom(1) {}
    
    /**
     * Sets the parameters to information contained in the given xml node.
     *
     * @param node Xml node containing parameter information for the Volume.
     */
    void setToXml(const XMLTree::Node *node)
    {
      near   = node->attributeAsFloat("near");
      far    = node->attributeAsFloat("far");
      left   = node->attributeAsFloat("left");
      right  = node->attributeAsFloat("right");
      top    = node->attributeAsFloat("top");
      bottom = node->attributeAsFloat("bottom");
    }
    
    /**
     * Calculates the width of the volume.
     *
     * @return Width of the volume.
     */
    float width() const {return right - left;}
    
    /**
     * Calculates the height of the volume.
     *
     * @return Height of the volume.
     */
    float height() const {return top - bottom;}
    
    float near, far, left, right, top, bottom;
  };
  
  /**
   * Manages the projection matrixices and viewports in an Object's RenderSlots.
   */
  class Projection: public Component
  {
  public:
    /**
     * Parses a Projection type enum value from the given string.
     *
     * @param str String value to parse.
     * @return Projection type enum value.
     */
    static PROJ_TYPE ParseProjectionType(const std::string &str)
    {
      if (str == "ortho")
        return PROJ_ORTHO;
      return PROJ_FRUSTUM;
    }
    
    /**
     * Parses an Aspect type enum value from the given string.
     *
     * @param str String value to parse.
     * @return Aspect type enum value.
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
     * Constructor
     *
     * @param scene Pointer to the scene that owns this.
     */
    Projection(Scene *scene): Component(scene),
      mType(PROJ_ORTHO),
      mAspect(ASPECT_NONE),
      mLock(0),
      mDisparity(0),
      mZeroDistance(1.0f)
    {
    }
    
    /**
     * Destructor
     */
    virtual ~Projection() {}
    
    /**
     * Sets the information in the given xml node to this Projection.
     *
     * @param node Xml node to set this Projection to.
     */
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
     * Setter for the Volume.
     *
     * @param volume Unadjusted Volume to be used by this Projection.
     */
    void setVolume(const Volume &volume)
    {
      lock();
      mVolume = volume;
      unlock();
    }
    
    /**
     * Getter for the unadjusted Volume to be used by this Projection.
     *
     * @return Volume used by this Projection.
     */
    Volume volume() const
    {
      lock();
      Volume ret = mVolume;
      unlock();
      return ret;
    }
    
    /**
     * Setter for setting the type of Projection by string value.
     *
     * @param str String value to parse the Projection type from.
     */
    void setType(const std::string &str)
    {
      setType(ParseProjectionType(str));
    }
    
    /**
     * Setter for setting the type of Projection, either Orthographic or Frustum.
     *
     * @param type Projection type enum value.
     */
    void setType(PROJ_TYPE type)
    {
      lock();
      mType = type;
      unlock();
    }
    
    /**
     * Getter for getting the type of Projection, either Orthographic or Frustum.
     *
     * @return Projection type enum value.
     */
    PROJ_TYPE type() const
    {
      lock();
      PROJ_TYPE ret = mType;
      unlock();
      return ret;
    }
    
    /**
     * Setter for setting the Aspect type by string value.
     *
     * @param str String to parse the Aspect type from.
     */
    void setAspect(const std::string &str) {setAspect(ParseAspectType(str));}
    
    /**
     * Setter for setting the aspect type used by this projection.
     *
     * @param aspect Aspect type enum value
     */
    void setAspect(ASPECT_TYPE aspect)
    {
      lock();
      mAspect = aspect;
      unlock();
    }
    
    /**
     * Getter for the aspect type used by this projection.
     *
     * @return Aspect type enum value
     */
    ASPECT_TYPE aspect() const
    {
      lock();
      ASPECT_TYPE ret = mAspect;
      unlock();
      return ret;
    }
    
    /**
     * Setter for the disparity or eye seperaation distance.
     *
     * @param disparity Disparity value
     */
    void setDisparity(float disparity)
    {
      lock();
      mDisparity = disparity;
      unlock();
    }
    
    /**
     * Getter for the disparity or eye seperaation distance.
     *
     * @return Disparity value
     */
    float disparity() const
    {
      lock();
      float ret = mDisparity;
      unlock();
      return ret;
    }
    
    /**
     * Setter for distance to the stereo zero plane.
     *
     * @param dist Distance to the stereo zero plane.
     */
    void setZeroDistance(float dist)
    {
      lock();
      mZeroDistance = dist;
      unlock();
    }
    
    /**
     * Getter for distance to the stereo zero plane.
     *
     * @return Distance to the stereo zero plane.
     */
    float zeroDistance() const
    {
      lock();
      float ret = mZeroDistance;
      unlock();
      return ret;
    }
    
    /**
     * Locks reading and writing to the fields.
     */
    void lock() const {SDL_AtomicLock(&mLock);}
    
    /**
     * Unlocks reading and writing to the fields.
     */
    void unlock() const {SDL_AtomicUnlock(&mLock);}
    
    /**
     * Creates a projection matrix from the given frame size and projection flags.
     *
     * @param size 2d vector for the size of the frame to create the projection for.
     * @param flags Projection flags for adjusting how the projection is created.
     * @return 4x4 matrix encoded with the projection information.
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
      
      int rotation = flags & PROJ_ROTATION;
      if (rotation == PROJ_ROT_CW || rotation == PROJ_ROT_CCW)
        size = size.yx();
      
      applyDisparity(v, disparity, zeroDistance, flags);
      applyAspect(v, size, aspect);
      
      mat4 modification = getModification(flags);
      
      if (type == PROJ_ORTHO)
        return modification * mat4::Ortho(v.left, v.right, v.bottom, v.top, v.near, v.far);
      return modification * mat4::Frustum(v.left, v.right, v.bottom, v.top, v.near, v.far) * mat4::Trans3d(vec3(disparity, 0.0f, 0.0f));
    }
    
    void adjustSlotViewport(RenderSlot &slot) const
    {
      int flags = slot.projectionFlags();
      Frame *frame = GetResource<Frame>(&slot.targets());
      if (frame && flags & PROJ_SPLIT)
        applySplit(slot, frame->size());
    }
    
  private:
    /**
     * Applys the projection matrix and frame spliting to the given RenderSlot.
     *
     * @param [in, out] slot RenderSlot to adjust
     */
    void applyToSlot(RenderSlot &slot) const
    {
      adjustSlotViewport(slot);
      
      // Then apply the projection matrix.
      applyProjection(slot);
    }
    
    /**
     * Adjusts the Viewport in the given RenderSlot to it's projection flags.
     *
     * @param [in,out] slot RenderSlot to apply the split on.
     * @param size 2d vector for the size of the Frame to split.
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
     * Applies a projection matrix to the given RenderSlot.
     *
     * @param [in,out] slot RenderSlot to apply the projection matrix.
     */
    void applyProjection(RenderSlot &slot) const
    {
      vec2 size(slot.drawState().viewport.size);
      if (size == vec2())
      {
        Frame *frame = GetResource<Frame>(&slot.targets());
        size = frame ? vec2(frame->size()) : vec2(1.0f, 1.0f);
      }
      
      mat4 projection = getProjection(size, slot.projectionFlags());
      slot.setUniform("Projection", projection);
    }
    
    /**
     * Creates a modification matrix from the given flags.
     *
     * @param flags Projection flags with modification information.
     * @return 4x4 matrix encoded with the modifications.
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
     * Adjusts the given volume and disparity for stereo rendering.
     *
     * @param [in,out] volume Volume to adjust for disparity.
     * @param [in,out] disparity Eye distance value to adjust.
     * @param zeroDistance Distance to the zero plane.
     * @param flags Projection flags to get stereo information from.
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
     * Adjusts the given volume to the given size and aspect type.
     *
     * @param [in,out] volume Volume to adjust.
     * @param size 2d vector for the frame size.
     * @param aspect Enum value for the type of aspect to adjust with.
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
    
    PROJ_TYPE   mType;   /**< Projection type, either Orthographic or Frustum. */
    ASPECT_TYPE mAspect; /**< Aspect type used to scale the projection to the frame. */
    Volume      mVolume; /**< Parameters to the parts of the projection. */
    
    float mDisparity;    /**< Distance between the virutal eyes for stereo rendering. */
    float mZeroDistance; /**< Distance to the zero plane used for stereo rendering. */
    
    mutable SDL_SpinLock mLock; /**< Spin lock used as a mutex for modifing the above fields. */
  };
}

#endif /* Projection_h */
