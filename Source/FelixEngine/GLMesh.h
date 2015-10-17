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
    GLMesh(GLGraphicSystem *system): mGLSystem(system), mIndexBuffer(0) {}
    virtual ~GLMesh() {clearBuffers();}
    
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
    bool load()
    {
      bool success = false;
      if (!isLoaded())
      {
        success = true;
        clearBuffers();
        
        glGenVertexArrays(1, &mVertexArray);
        glBindVertexArray(mVertexArray);
        for (VertexBufferMap::const_iterator buffer = mBufferMap.begin(); buffer != mBufferMap.end(); ++buffer)
          success &= loadVertexBuffer(buffer->first, buffer->second);
        if (mBufferMap.indexBuffer().size())
          success &= loadIndexBuffer(mBufferMap.indexBuffer());
        glBindVertexArray(0);
        
        const Ranges &subMeshes = mBufferMap.subMeshes();
        if (subMeshes.size())
        {
          for (Ranges::const_iterator range = subMeshes.begin(); range != subMeshes.end(); ++range)
          {
            mSubMeshes.push_back(GLRange());
            mSubMeshes.back().start = range->start;
            mSubMeshes.back().end = range->end;
          }
        }
        else
        {
          mSubMeshes.push_back(GLRange());
          mSubMeshes.back().end = mBufferMap.count();
        }
        setGLPrimitiveType(mBufferMap.primitiveType());
      }
      return success;
    }
    
    void clearBuffers()
    {
      // Clear the Vertex Buffers
      for (GLBufferMap::iterator buffer = mVertexBuffers.begin(); buffer != mVertexBuffers.end(); ++buffer)
        glDeleteBuffers(1, &buffer->second.bufferId);
      mVertexBuffers.clear();
      
      // Clear the Index Buffer
      if (mIndexBuffer)
      {
        glDeleteBuffers(1, &mIndexBuffer);
        mIndexBuffer = 0;
      }
      mSubMeshes.clear();
    }
    
    bool loadVertexBuffer(const std::string &name, const VertexBuffer &buffer)
    {
      // Create the Vertex Buffer
      GLBuffer glBuffer;
      glGenBuffers(1, &glBuffer.bufferId);
      if (!glBuffer.bufferId)
      {
        std::cerr << "Error Creating Vertex Buffer." << std::endl;
        return false;
      }
      glBuffer.components = buffer.components();
      glBuffer.count = buffer.count();
      
      // Load the Vertices
      glBindBuffer(GL_ARRAY_BUFFER, glBuffer.bufferId);
      GLsizeiptr bufSize = glBuffer.components * glBuffer.count * sizeof(float);
      const GLvoid *ptr = (const GLvoid*)buffer.ptr();
      glBufferData(GL_ARRAY_BUFFER, bufSize, ptr, GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      
      mVertexBuffers[name] = glBuffer;
      return true;
    }
    
    bool loadIndexBuffer(const IndexBuffer &buffer)
    {
      // Create the Index Buffer
      glGenBuffers(1, &mIndexBuffer);
      if (!mIndexBuffer)
      {
        std::cerr << "Error Creating Index Buffer." << std::endl;
        return false;
      }
      
      // Load the Indices.
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
      GLsizeiptr bufSize = buffer.size() * sizeof(unsigned int);
      const GLvoid *ptr = (const GLvoid*)&buffer.at(0);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,  bufSize, ptr, GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      
      return true;
    }
    
    void setGLPrimitiveType(VERTEX_PRIMITIVE type)
    {
      if (type == VERTEX_TRIANGLES)
        mPrimitiveType = GL_TRIANGLES;
      if (type == VERTEX_TRIANGLE_STRIP)
        mPrimitiveType = GL_TRIANGLE_STRIP;
    }
    
    GLGraphicSystem *mGLSystem;
    
    GLBufferMap mVertexBuffers;
    GLuint      mIndexBuffer;
    GLuint      mVertexArray;
    GLenum      mPrimitiveType;
    GLRanges    mSubMeshes;
  };
}

#endif /* GLMesh_h */
