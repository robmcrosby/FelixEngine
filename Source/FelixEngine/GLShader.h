//
//  GLShader.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GLShader_h
#define GLShader_h

#include "GraphicSystem.h"


namespace fx
{
  class GLGraphicSystem;
  
  /**
   *
   */
  class GLShader: public Shader
  {
  public:
    GLShader(GLGraphicSystem *system);
    virtual ~GLShader();
    
    bool load();
    
    virtual void setVertexShaderSrc(const std::string &src);
    virtual void setFragmentShaderSrc(const std::string &src);
    
  private:
    GLGraphicSystem *mGLSystem;
  };
}

#endif /* GLShader_h */
