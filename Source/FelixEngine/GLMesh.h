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
   * Resource that manages OpenGL Vertex and Index buffers that make up Meshes.
   */
  class GLMesh: public Mesh
  {
  private:
    typedef Vector2<GLuint> GLRange;       /**< define a 2d vector using OpenGL's primitives. */
    typedef std::vector<GLRange> GLRanges; /**< define of a vector of GLRanges */
    
    /**
     * Internal struct that bundles information about an OpenGL Vertex buffer.
     */
    struct GLBuffer
    {
      /**
       * Constructor initalizes all the fields to zero.
       */
      GLBuffer(): components(0), count(0), bufferId(0) {}
      GLuint components; /**< Number of components in each Vertex Item */
      GLuint count;      /**< Number of Vertex Items in the buffer */
      GLuint bufferId;   /**< OpenGL Vertex buffer handle */
    };
    typedef std::map<std::string, GLBuffer> GLBufferMap; /**< Defines a string to GLBuffer map. */
    
  public:
    /**
     * Constructor that initalizes the Mesh.
     *
     * @param system Pointer to a GLGraphicsSystem.
     */
    GLMesh(GLGraphicSystem *system): mGLSystem(system), mIndexBuffer(0) {}
    
    /**
     * Destructor that calls unload.
     */
    virtual ~GLMesh() {unload();}
    
    /**
     * Binds the Mesh to the given shader.
     *
     * @param shader Shader to bind this Mesh to.
     */
    void bind(const GLShader *shader) const
    {
      if (shader)
      {
        glBindVertexArray(mVertexArray);
        for (const auto &buffer : mVertexBuffers)
        {
          GLint index = shader->getAttributeIndex(buffer.first);
          if (index >= 0)
          {
            glBindBuffer(GL_ARRAY_BUFFER, buffer.second.bufferId);
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, buffer.second.components, GL_FLOAT, GL_FALSE, 0, 0);
          }
        }
      }
    }
    
    /**
     * Draws the Mesh using OpenGL calls.
     *
     * @param instances Number of times the mesh would be drawn.
     * @param index The sub-mesh to draw.
     */
    void draw(int instances, int index) const
    {
      if (index < (int)mSubMeshes.size())
      {
        const GLRange &subMesh = mSubMeshes.at(index);
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
    
    /**
     * Uploads the Vertex and Index information from the given Buffer Map to
     * Video Memory.
     *
     * @param bufferMap Refrence to the BufferMap to load from.
     */
    void uploadBufferMap(Buffer &bufferMap)
    {
      bool success = true;
      unload();
      
      glGenVertexArrays(1, &mVertexArray);
      glBindVertexArray(mVertexArray);
      
      for (const Buffer &buffer : bufferMap)
      {
        if (buffer.bufferType() == BUFFER_VERTEX)
          success &= uploadVertexBuffer(buffer);
        else if (buffer.bufferType() == BUFFER_INDICES)
          success &= uploadIndexBuffer(buffer);
        else if (buffer.bufferType() == BUFFER_RANGES)
          success &= setSubMeshes(buffer);
      }
      glBindVertexArray(0);
      
      if (mSubMeshes.size() == 0)
      {
        mSubMeshes.push_back(GLRange());
        mSubMeshes.back().end = mVertexBuffers.begin()->second.count;
      }
      setGLPrimitiveType((VERTEX_PRIMITIVE)bufferMap.flags());
      setLoaded(success);
    }
    
    /**
     * Deletes the Vertex/Index buffers from Video Memory.
     */
    void unload()
    {
      // Clear the Vertex Buffers
      for (const auto &buffer : mVertexBuffers)
        glDeleteBuffers(1, &buffer.second.bufferId);
      mVertexBuffers.clear();
      
      // Clear the Index Buffer
      if (mIndexBuffer)
      {
        glDeleteBuffers(1, &mIndexBuffer);
        mIndexBuffer = 0;
      }
      mSubMeshes.clear();
      setUnloaded();
    }
    
  private:
    /**
     * Uploads a Vertex Buffer from the given Buffer to Video Memory.
     *
     * @param buffer Refrence to the Buffer to upload.
     * @return true if successful or false on an error.
     */
    bool uploadVertexBuffer(const Buffer &buffer)
    {
      // Create the Vertex Buffer
      GLBuffer glBuffer;
      glGenBuffers(1, &glBuffer.bufferId);
      if (!glBuffer.bufferId)
      {
        std::cerr << "Error Creating Vertex Buffer." << std::endl;
        return false;
      }
      
      // Determine the number of components
      if (buffer.type() == VAR_FLOAT)
        glBuffer.components = 1;
      else if (buffer.type() == VAR_FLOAT_2)
        glBuffer.components = 2;
      else if (buffer.type() == VAR_FLOAT_4)
        glBuffer.components = 4;
      
      glBuffer.count = (GLuint)buffer.size();
      
      // Load the Vertices
      glBindBuffer(GL_ARRAY_BUFFER, glBuffer.bufferId);
      GLsizeiptr bufSize = glBuffer.components * glBuffer.count * sizeof(float);
      const GLvoid *ptr = (const GLvoid*)buffer.ptr();
      glBufferData(GL_ARRAY_BUFFER, bufSize, ptr, GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      
      mVertexBuffers[buffer.name()] = glBuffer;
      return true;
    }
    
    /**
     * Uploads an Index Buffer from the given Buffer to Video Memory.
     *
     * @param buffer Refrence to the Buffer to upload.
     * @return true if successful or false on an error.
     */
    bool uploadIndexBuffer(const Buffer &buffer)
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
      const GLvoid *ptr = (const GLvoid*)buffer.ptr();
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,  bufSize, ptr, GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      
      return true;
    }
    
    /**
     * Sets the Sub-Mesh ranges from the given Buffer.
     *
     * @param buffer Refrence to the Buffer to load the ranges from.
     * @return true if there were ranges loaded or false if not.
     */
    bool setSubMeshes(const Buffer &buffer)
    {
      mSubMeshes.clear();
      ivec2 *ranges = (ivec2*)buffer.ptr();
      for (int i = 0; i < buffer.size(); ++i)
        mSubMeshes.push_back(ranges[i]);
      return mSubMeshes.size() > 0;
    }
    
    /**
     * Sets the OpenGL primitive type from the given type.
     *
     * @param type Enum to either VERTEX_TRIANGLES or GL_TRIANGLE_STRIP.
     */
    void setGLPrimitiveType(VERTEX_PRIMITIVE type)
    {
      if (type == VERTEX_TRIANGLES)
        mPrimitiveType = GL_TRIANGLES;
      else if (type == VERTEX_TRIANGLE_STRIP)
        mPrimitiveType = GL_TRIANGLE_STRIP;
    }
    
    GLBufferMap mVertexBuffers; /**< Map of Vertex Buffers */
    GLuint      mIndexBuffer;   /**< OpenGL Index Buffer handle */
    GLuint      mVertexArray;   /**< OpenGL Vertex Array handle */
    GLenum      mPrimitiveType; /**< OpenGL Enum for Primitive Type */
    GLRanges    mSubMeshes;     /**< Vector of Ranges for Sub-Meshes */
    
    GLGraphicSystem *mGLSystem; /**< Pointer to the GLGraphicsSystem that owns this Mesh */
  };
}

#endif /* GLMesh_h */
