//
//  GLMesh.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GLMesh_h
#define GLMesh_h

#include "GLGraphicSystem.h"
#include "GLShader.h"
#include <vector>
#include <map>

namespace fx
{
  /**
   *
   */
  class GLMesh: public Mesh
  {
  private:
    struct GLRange
    {
      GLRange(): start(0), end(0) {}
      GLuint start, end;
    };
    typedef std::vector<GLRange> GLRanges;
    
    struct GLBuffer
    {
      GLBuffer(): components(0), count(0), bufferId(0) {}
      GLuint components, count, bufferId;
    };
    typedef std::map<std::string, GLBuffer> GLBufferMap;
    
  public:
    GLMesh(GLGraphicSystem *system);
    virtual ~GLMesh();
    
    void update()
    {
      if (isLoading())
      {
        if (load())
          setLoaded();
        else
          setNotLoading();
      }
    }
    
    void draw(const GLShader *shader, int instances, int index) const
    {
      if (shader && index < (int)mSubMeshes.size())
      {
        const GLRange &subMesh = mSubMeshes.at(index);
        glBindVertexArray(mVertexArray);
        for (GLBufferMap::const_iterator itr = mVertexBuffers.begin(); itr != mVertexBuffers.end(); ++itr)
        {
          GLint index = shader->getAttributeIndex(itr->first);
          if (index >= 0)
          {
            glBindBuffer(GL_ARRAY_BUFFER, itr->second.bufferId);
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, itr->second.components, GL_FLOAT, GL_FALSE, 0, 0);
          }
        }
        if (mIndexBuffer)
        {
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
          GLsizei count = (GLsizei)(subMesh.end - subMesh.start);
          GLvoid* ptr = (GLvoid*)(subMesh.start*sizeof(GLuint));
          GLsizei primCount = (GLsizei)instances;
          glDrawElementsInstanced(mPrimitiveType, count, GL_UNSIGNED_INT, ptr, primCount);
        }
        else
        {
          GLint first = (GLint)subMesh.start;
          GLsizei count = (GLsizei)(subMesh.end - subMesh.start);
          GLsizei primCount = (GLsizei)instances;
          glDrawArraysInstanced(mPrimitiveType, first, count, primCount);
        }
        glBindVertexArray(0);
      }
    }
    
  private:
    bool load();
    void clearBuffers();
    bool loadVertexBuffer(const std::string &name, const VertexBuffer &buffer);
    bool loadIndexBuffer(const IndexBuffer &buffer);
    void setGLPrimitiveType(VERTEX_PRIMITIVE type);
    
    void createVertexArray();
    
    GLGraphicSystem *mGLSystem;
    
    GLBufferMap mVertexBuffers;
    GLuint      mIndexBuffer;
    GLuint      mVertexArray;
    GLenum      mPrimitiveType;
    GLRanges    mSubMeshes;
  };
}

#endif /* GLMesh_h */
