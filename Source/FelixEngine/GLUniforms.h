//
//  GLUniforms.h
//  FelixEngine
//
//  Created by Robert Crosby on 1/19/16.
//  Copyright © 2016 Robert Crosby. All rights reserved.
//

#ifndef GLUniforms_h
#define GLUniforms_h

#include "GLBuffer.h"
#include "Buffer.h"

namespace fx
{
  class GLUniforms: public Resource
  {
  public:
    GLUniforms() {}
    virtual ~GLUniforms() {}
    
    void uploadBufferMap(const Buffer &bufferMap)
    {
      IndexedMap<Buffer> *map = bufferMap.mapPtr();
      if (map)
      {
        for (const auto buffer : *map)
        {
          
        }
      }
      
      mBufferMap = bufferMap;
    }
    
    void applyToShader(const GLShader *shader)
    {
      if (shader)
      {
        for (Buffer &buffer : mBufferMap)
        {
          if (buffer.bufferType() == BUFFER_UNIFORM)
            applyUniformToShader(buffer, shader);
          else if (buffer.bufferType() == BUFFER_STRUCT)
            applyStructToShader(buffer, shader);
        }
      }
    }
    
  private:
    void applyUniformToShader(Buffer &buffer, const GLShader *shader) const
    {
      GLint loc = shader->getUniformLocation(buffer.name());
      if (loc != -1)
      {
        switch (buffer.type())
        {
          case VAR_FLOAT:
            glUniform1fv(loc, (GLsizei)buffer.size(), (const GLfloat*)buffer.ptr());
            break;
          case VAR_INT:
            glUniform1iv(loc, (GLsizei)buffer.size(), (const GLint*)buffer.ptr());
            break;
          case VAR_FLOAT_2:
            glUniform2fv(loc, (GLsizei)buffer.size(), (const GLfloat*)buffer.ptr());
            break;
          case VAR_INT_2:
            glUniform2iv(loc, (GLsizei)buffer.size(), (const GLint*)buffer.ptr());
            break;
          case VAR_FLOAT_3:
            glUniform3fv(loc, (GLsizei)buffer.size(), (const GLfloat*)buffer.ptr());
            break;
          case VAR_INT_3:
            glUniform3iv(loc, (GLsizei)buffer.size(), (const GLint*)buffer.ptr());
            break;
          case VAR_FLOAT_4:
            glUniform4fv(loc, (GLsizei)buffer.size(), (const GLfloat*)buffer.ptr());
            break;
          case VAR_INT_4:
            glUniform4iv(loc, (GLsizei)buffer.size(), (const GLint*)buffer.ptr());
            break;
          case VAR_MTX_2X2:
            glUniformMatrix2fv(loc, (GLsizei)buffer.size(), GL_FALSE, (const GLfloat*)buffer.ptr());
            break;
          case VAR_MTX_3X3:
            glUniformMatrix3fv(loc, (GLsizei)buffer.size(), GL_FALSE, (const GLfloat*)buffer.ptr());
            break;
          case VAR_MTX_4X4:
            glUniformMatrix4fv(loc, (GLsizei)buffer.size(), GL_FALSE, (const GLfloat*)buffer.ptr());
            break;
          case VAR_CHAR_4:
          case VAR_STRING:
          case VAR_UNKNOWN:
            break;
        }
      }
    }
    
    void applyStructToShader(Buffer &bufferMap, const GLShader *shader) const
    {
      for (Buffer &buffer : bufferMap)
      {
        std::string name = bufferMap.name() + "." + buffer.name();
        GLint loc = shader->getUniformLocation(name);
        if (loc != -1)
        {
          switch (buffer.type())
          {
            case VAR_FLOAT:
              glUniform1fv(loc, (GLsizei)buffer.size(), (const GLfloat*)buffer.ptr());
              break;
            case VAR_INT:
              glUniform1iv(loc, (GLsizei)buffer.size(), (const GLint*)buffer.ptr());
              break;
            case VAR_FLOAT_2:
              glUniform2fv(loc, (GLsizei)buffer.size(), (const GLfloat*)buffer.ptr());
              break;
            case VAR_INT_2:
              glUniform2iv(loc, (GLsizei)buffer.size(), (const GLint*)buffer.ptr());
              break;
            case VAR_FLOAT_3:
              glUniform3fv(loc, (GLsizei)buffer.size(), (const GLfloat*)buffer.ptr());
              break;
            case VAR_INT_3:
              glUniform3iv(loc, (GLsizei)buffer.size(), (const GLint*)buffer.ptr());
              break;
            case VAR_FLOAT_4:
              glUniform4fv(loc, (GLsizei)buffer.size(), (const GLfloat*)buffer.ptr());
              break;
            case VAR_INT_4:
              glUniform4iv(loc, (GLsizei)buffer.size(), (const GLint*)buffer.ptr());
              break;
            case VAR_MTX_2X2:
              glUniformMatrix2fv(loc, (GLsizei)buffer.size(), GL_FALSE, (const GLfloat*)buffer.ptr());
              break;
            case VAR_MTX_3X3:
              glUniformMatrix3fv(loc, (GLsizei)buffer.size(), GL_FALSE, (const GLfloat*)buffer.ptr());
              break;
            case VAR_MTX_4X4:
              glUniformMatrix4fv(loc, (GLsizei)buffer.size(), GL_FALSE, (const GLfloat*)buffer.ptr());
              break;
            case VAR_CHAR_4:
            case VAR_STRING:
            case VAR_UNKNOWN:
              break;
          }
        }
      }
    }
    
    Buffer mBufferMap;
  };
}


#endif /* GLUniforms_h */
