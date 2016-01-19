//
//  GLUniformMap.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/12/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GLUniformMap_h
#define GLUniformMap_h

//#include "UniformMap.h"
//#include "GLShader.h"
//
//namespace fx
//{
//  class GLUniformMap: public InternalUniformMap
//  {
//  public:
//    GLUniformMap(UniformMap *map): mUniformMap(map) {}
//    virtual ~GLUniformMap() {clear();}
//    
//    virtual void release() {mUniformMap = nullptr;}
//    
//    void update()
//    {
//      if (inUse())
//      {
//        mUniformMap->lock();
//        
//        resize(mUniformMap->size());
//        mList.resize(mUniformMap->size());
//        UniformMap::iterator mapItr = mUniformMap->begin();
//        UniformList::iterator listItr = mList.begin();
//        while (mapItr != mUniformMap->end())
//        {
//          copyUniform(listItr, mapItr);
//          listItr++;
//          mapItr++;
//        }
//        
//        mUniformMap->unlock();
//      }
//    }
//    bool inUse() const {return mUniformMap;}
//    void applyToShader(const GLShader *shader) const
//    {
//      if (shader)
//      {
//        for (UniformList::const_iterator itr = mList.begin(); itr != mList.end(); ++itr)
//        {
//          if (itr->second.instanceDivisor() > 0)
//            applyBufferToShader(itr->first, itr->second, shader);
//          else
//            applyUniformToShader(itr->first, itr->second, shader);
//        }
//      }
//    }
//
//  private:
//    void copyUniform(UniformList::iterator &dst, UniformMap::iterator &src)
//    {
//      if (dst->first != src->first)
//        clearUniformBuffer(dst->second);
//      if (src->second.instanceDivisor() > 0)
//      {
//        if (dst->second != src->second)
//        {
//          clearUniformBuffer(dst->second);
//          createUniformBuffer(src->second);
//        }
//      }
//      else
//        clearUniformBuffer(dst->second);
//      
//      *dst = *src;
//    }
//    void resize(size_t size)
//    {
//      while (size < mList.size())
//      {
//        clearUniformBuffer(mList.back().second);
//        mList.pop_back();
//      }
//      mList.resize(size);
//    }
//    void clear()
//    {
//      for (UniformList::iterator itr = mList.begin(); itr != mList.end(); ++itr)
//        clearUniformBuffer(itr->second);
//      mList.clear();
//    }
//    void createUniformBuffer(Uniform &uniform)
//    {
//      GLuint bufferId;
//      glGenBuffers(1, &bufferId);
//      if (!bufferId)
//        std::cerr << "Error Creating Uniform Buffer." << std::endl;
//      else
//      {
//        glBindBuffer(GL_ARRAY_BUFFER, bufferId);
//        GLsizeiptr bufSize = uniform.sizeInBytes();
//        const GLvoid *ptr = (const GLvoid*)uniform.ptr();
//        glBufferData(GL_ARRAY_BUFFER, bufSize, ptr, GL_STATIC_DRAW);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//      }
//      uniform.setBufferId(bufferId);
//    }
//    void clearUniformBuffer(Uniform &uniform)
//    {
//      GLuint bufferId = uniform.bufferId();
//      if (bufferId > 0)
//        glDeleteBuffers(1, &bufferId);
//      uniform.setBufferId(0);
//    }
//    void applyBufferToShader(const std::string &name, const Uniform &uniform, const GLShader *shader) const
//    {
//      GLint loc = shader->getAttributeIndex(name);
//      if (loc != -1)
//      {
//        GLuint bufferId = uniform.bufferId();
//        if (bufferId > 0)
//        {
//          glBindBuffer(GL_ARRAY_BUFFER, bufferId);
//          switch (uniform.type())
//          {
//            case VAR_FLOAT:
//              glVertexAttribPointer(loc, 1, GL_FLOAT, GL_FALSE, 0, NULL);
//              break;
//            case VAR_INT:
//              glVertexAttribPointer(loc, 1, GL_INT, GL_FALSE, 0, NULL);
//              break;
//            case VAR_FLOAT_2:
//              glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, NULL);
//              break;
//            case VAR_INT_2:
//              glVertexAttribPointer(loc, 2, GL_INT, GL_FALSE, 0, NULL);
//              break;
//            case VAR_FLOAT_3:
//              glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
//              break;
//            case VAR_INT_3:
//              glVertexAttribPointer(loc, 3, GL_INT, GL_FALSE, 0, NULL);
//              break;
//            case VAR_FLOAT_4:
//              glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, NULL);
//              break;
//            case VAR_INT_4:
//              glVertexAttribPointer(loc, 4, GL_INT, GL_FALSE, 0, NULL);
//              break;
//            case VAR_MTX_2X2:
//              glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, NULL);
//              break;
//            case VAR_MTX_3X3:
//              glVertexAttribPointer(loc, 9, GL_FLOAT, GL_FALSE, 0, NULL);
//              break;
//            case VAR_MTX_4X4:
//              glVertexAttribPointer(loc, 16, GL_FLOAT, GL_FALSE, 0, NULL);
//              break;
//            case VAR_UNKNOWN:
//              break;
//          }
//          glEnableVertexAttribArray(loc);
//          glVertexAttribDivisor(loc, uniform.instanceDivisor());
//        }
//      }
//      else
//        applyUniformToShader(name, uniform, shader);
//    }
//    void applyUniformToShader(const std::string &name, const Uniform &uniform, const GLShader *shader) const
//    {
//      GLint loc = shader->getUniformLocation(name);
//      if (loc != -1)
//      {
//        switch (uniform.type())
//        {
//          case VAR_FLOAT:
//            glUniform1fv(loc, (GLsizei)uniform.size(), (const GLfloat*)uniform.ptr());
//            break;
//          case VAR_INT:
//            glUniform1iv(loc, (GLsizei)uniform.size(), (const GLint*)uniform.ptr());
//            break;
//          case VAR_FLOAT_2:
//            glUniform2fv(loc, (GLsizei)uniform.size(), (const GLfloat*)uniform.ptr());
//            break;
//          case VAR_INT_2:
//            glUniform2iv(loc, (GLsizei)uniform.size(), (const GLint*)uniform.ptr());
//            break;
//          case VAR_FLOAT_3:
//            glUniform3fv(loc, (GLsizei)uniform.size(), (const GLfloat*)uniform.ptr());
//            break;
//          case VAR_INT_3:
//            glUniform3iv(loc, (GLsizei)uniform.size(), (const GLint*)uniform.ptr());
//            break;
//          case VAR_FLOAT_4:
//            glUniform4fv(loc, (GLsizei)uniform.size(), (const GLfloat*)uniform.ptr());
//            break;
//          case VAR_INT_4:
//            glUniform4iv(loc, (GLsizei)uniform.size(), (const GLint*)uniform.ptr());
//            break;
//          case VAR_MTX_2X2:
//            glUniformMatrix2fv(loc, (GLsizei)uniform.size(), GL_FALSE, (const GLfloat*)uniform.ptr());
//            break;
//          case VAR_MTX_3X3:
//            glUniformMatrix3fv(loc, (GLsizei)uniform.size(), GL_FALSE, (const GLfloat*)uniform.ptr());
//            break;
//          case VAR_MTX_4X4:
//            glUniformMatrix4fv(loc, (GLsizei)uniform.size(), GL_FALSE, (const GLfloat*)uniform.ptr());
//            break;
//          case VAR_UNKNOWN:
//            break;
//        }
//      }
//    }
//
//    UniformMap *mUniformMap;
//    UniformList mList;
//  };
//}

#endif /* GLUniformMap_h */
