//
//  GLUniformMap.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/12/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GLUniformMap_h
#define GLUniformMap_h

#include "UniformMap.h"
#include "GLShader.h"

namespace fx
{
  class GLUniformMap: public InternalUniformMap
  {
  public:
    GLUniformMap() {}
    virtual ~GLUniformMap() {}
    
    virtual void update(const VariantMap &map) {mMap = map;}
    void applyToShader(const GLShader *shader) const
    {
      if (shader)
      {
        for (VariantMap::const_iterator itr = mMap.begin(); itr != mMap.end(); ++itr)
          applyUniformToShader(itr->first, itr->second, shader);
      }
    }
    
  private:
    void applyUniformToShader(const std::string &name, const Uniform &uniform, const GLShader *shader) const
    {
      GLint loc = shader->getUniformLocation(name);
      if (loc != -1)
      {
        switch (uniform.type())
        {
          case VAR_FLOAT:
            glUniform1fv(loc, (GLsizei)uniform.size(), (const GLfloat*)uniform.ptr());
            break;
          case VAR_INT:
            glUniform1iv(loc, (GLsizei)uniform.size(), (const GLint*)uniform.ptr());
            break;
          case VAR_FLOAT_2:
            glUniform2fv(loc, (GLsizei)uniform.size(), (const GLfloat*)uniform.ptr());
            break;
          case VAR_INT_2:
            glUniform2iv(loc, (GLsizei)uniform.size(), (const GLint*)uniform.ptr());
            break;
          case VAR_FLOAT_3:
            glUniform3fv(loc, (GLsizei)uniform.size(), (const GLfloat*)uniform.ptr());
            break;
          case VAR_INT_3:
            glUniform3iv(loc, (GLsizei)uniform.size(), (const GLint*)uniform.ptr());
            break;
          case VAR_FLOAT_4:
            glUniform4fv(loc, (GLsizei)uniform.size(), (const GLfloat*)uniform.ptr());
            break;
          case VAR_INT_4:
            glUniform4iv(loc, (GLsizei)uniform.size(), (const GLint*)uniform.ptr());
            break;
          case VAR_MTX_2X2:
            glUniformMatrix2fv(loc, (GLsizei)uniform.size(), GL_FALSE, (const GLfloat*)uniform.ptr());
            break;
          case VAR_MTX_3X3:
            glUniformMatrix3fv(loc, (GLsizei)uniform.size(), GL_FALSE, (const GLfloat*)uniform.ptr());
            break;
          case VAR_MTX_4X4:
            glUniformMatrix4fv(loc, (GLsizei)uniform.size(), GL_FALSE, (const GLfloat*)uniform.ptr());
            break;
          case VAR_UNKNOWN:
            break;
        }
      }
    }

    VariantMap mMap;
  };
}

#endif /* GLUniformMap_h */
