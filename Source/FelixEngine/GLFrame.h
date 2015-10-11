//
//  GLFrame.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GLFrame_h
#define GLFrame_h

#include "GLGraphicSystem.h"


namespace fx
{
  /**
   *
   */
  class GLFrame: public Frame
  {
  public:
    GLFrame(GLGraphicSystem *system);
    virtual ~GLFrame();
    
    virtual void reload();
    
    virtual void addBuffer(BUFFER_TYPE type, const std::string &name, const Sampler &sampler);
    virtual void setSize(const ivec2 &size);
    
    bool load();
    
  private:
    GLGraphicSystem *mGLSystem;
  };
}

#endif /* GLFrame_h */
