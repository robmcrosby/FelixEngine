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
    
    virtual void reload();
    
    bool load();
    void draw(const GLShader *shader, int instances, int index) const
    {
      if (shader && index < (int)mSubMeshes.size())
      {
        const GLRange &subMesh = mSubMeshes.at(index);
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
          // TODO: get instancing working
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
          GLsizei size = (GLsizei)subMesh.end;
          GLvoid* ptr = (GLvoid*)(subMesh.start*sizeof(GLuint));
          glDrawElements(mPrimitiveType, size, GL_UNSIGNED_INT, ptr);
          
//          #ifdef GL_ES_VERSION_3_0
//          glDrawElementsInstanced(mPrimitiveType, mNumIndices, GL_UNSIGNED_INT, 0, (GLsizei)instances);
//          #else
//          glDrawElementsInstancedEXT(mPrimitiveType, mNumIndices, GL_UNSIGNED_INT, 0, (GLsizei)instances);
//          #endif
        }
        else
        {
          // TODO: get instancing working
          GLint first = (GLint)subMesh.start;
          GLsizei count = (GLsizei)(subMesh.end - subMesh.start);
          glDrawArrays(mPrimitiveType, first, count);
          
//          #ifdef GL_ES_VERSION_3_0
//          glDrawArraysInstanced(mPrimitiveType, 0, mNumVertices, (GLsizei)instances);
//          #else
//          glDrawArraysInstancedEXT(mPrimitiveType, 0, mNumVertices, (GLsizei)instances);
//          #endif
        }
      }
    }
    
  private:
    void clearBuffers();
    bool loadVertexBuffer(const std::string &name, const VertexBuffer &buffer);
    bool loadIndexBuffer(const IndexBuffer &buffer);
    void setGLPrimitiveType(VERTEX_PRIMITIVE type);
    
    GLGraphicSystem *mGLSystem;
    
    GLBufferMap mVertexBuffers;
    GLuint      mIndexBuffer;
    GLenum      mPrimitiveType;
    GLRanges    mSubMeshes;
  };
}

#endif /* GLMesh_h */
