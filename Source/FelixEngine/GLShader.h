//
//  GLShader.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GLShader_h
#define GLShader_h

#include "GLGraphicSystem.h"


namespace fx
{
  /**
   *
   */
  class GLShader: public Shader
  {
  public:
    GLShader(GLGraphicSystem *system);
    virtual ~GLShader();
    
    virtual void reload();
    
    virtual void setVertexShaderSrc(const std::string &src);
    virtual void setFragmentShaderSrc(const std::string &src);
    
    bool load();
    
  private:
    GLGraphicSystem *mGLSystem;
  };
}

#endif /* GLShader_h */
