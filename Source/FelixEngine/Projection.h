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
    
    void setVolume(const Volume &volume) {mVolume = volume;}
    Volume volume() const {return mVolume;}
    
    void setType(const std::string &str);
    void setType(PROJ_TYPE type) {mType = type;}
    PROJ_TYPE type() const {return mType;}
    
    void setAspect(const std::string &str);
    void setAspect(ASPECT_TYPE aspect) {mAspect = aspect;}
    ASPECT_TYPE aspect() const {return mAspect;}
    
    void setFrame(const std::string &name);
    void setFrame(Frame *frame);
    Frame* frame() const {return mFrame;}
    
  private:
    void update(void*);
    void updateMatrix();
    
    PROJ_TYPE   mType;
    ASPECT_TYPE mAspect;
    
    Volume mVolume;
    mat4   mMatrix;
    Frame *mFrame;
    
    RenderSlots *mRenderSlots;
  };
}

#endif /* Projection_h */
