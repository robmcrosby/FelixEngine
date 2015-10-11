//
//  GLFrame.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GLFrame_h
#define GLFrame_h

#include "GraphicSystem.h"


namespace fx
{
  class GLGraphicSystem;
  
  /**
   *
   */
  class GLFrame: public Frame
  {
  public:
    GLFrame(GLGraphicSystem *system);
    virtual ~GLFrame();
    
    bool load();
    
    virtual void addBuffer(BUFFER_TYPE type, const std::string &name, const Sampler &sampler);
    virtual void setSize(const ivec2 &size);
    
  private:
    GLGraphicSystem *mGLSystem;
  };
}

#endif /* GLFrame_h */
