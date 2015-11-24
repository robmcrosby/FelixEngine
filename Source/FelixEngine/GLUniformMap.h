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
    GLUniformMap(const UniformMap *map): mUniformMap(map) {}
    virtual ~GLUniformMap() {}
    
    virtual void release() {mUniformMap = nullptr;}
    
    void update()
    {
      if (inUse())
      {
        mUniformMap->lock();
        
        mList.resize(mUniformMap->size());
        UniformMap::iterator mapItr = mUniformMap->begin();
        VariantList::iterator listItr = mList.begin();
        while (mapItr != mUniformMap->end())
          *(listItr++) = *(mapItr++);
        
        mUniformMap->unlock();
      }
    }
    bool inUse() const {return mUniformMap;}
    void applyToShader(const GLShader *shader) const
    {
      if (shader)
      {
        for (VariantList::const_iterator itr = mList.begin(); itr != mList.end(); ++itr)
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
    
    const UniformMap *mUniformMap;
    VariantList mList;
  };
}

#endif /* GLUniformMap_h */
