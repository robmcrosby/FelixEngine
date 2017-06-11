//
//  GLBuffer.h
//  FelixEngine
//
//  Created by Robert Crosby on 3/30/16.
//  Copyright © 2016 Robert Crosby. All rights reserved.
//

#ifndef GLBuffer_h
#define GLBuffer_h

#include "GLShader.h"
#include "Buffer.h"

namespace fx
{
  class GLBuffer: public Resource
  {
  public:
    GLBuffer(): mBufferId(0) {}
    virtual ~GLBuffer() {}
    
    void upload(const Buffer &buffer)
    {
      mBuffer = buffer;
      if (buffer.bufferType() == BUFFER_INSTANCED)
        uploadInstanced(buffer);
    }
    
    void unload()
    {
    }
    
    void applyToShader(const GLShader *shader)
    {
      if (shader)
      {
        if (mBuffer.bufferType() == BUFFER_UNIFORM)
          applyUniform(shader);
        else if (mBuffer.bufferType() == BUFFER_STRUCT)
          applyStruct(shader);
        else if (mBuffer.bufferType() == BUFFER_INSTANCED)
          applyInstanced(shader);
      }
    }
    
  private:
    void uploadInstanced(const Buffer &buffer)
    {
      if (!mBufferId)
        unloadInstanced();
      glGenBuffers(1, &mBufferId);
      
      glBindBuffer(GL_ARRAY_BUFFER, mBufferId);
      glBufferData(GL_ARRAY_BUFFER, buffer.sizeInBytes(), buffer.ptr(), GL_DYNAMIC_DRAW);
    }
    
    void unloadInstanced()
    {
      glDeleteBuffers(1, &mBufferId);
      mBufferId = 0;
    }
    
    void applyInstanced(const GLShader *shader) const
    {
      GLint index = shader->getAttributeIndex(mBuffer.name());
      if (index != -1)
      {
        glBindBuffer(GL_ARRAY_BUFFER, mBufferId);
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, (GLint)mBuffer.components(), GL_FLOAT, GL_FALSE, 0, 0);
        glVertexAttribDivisor(index, 1);
      }
    }
    
    void applyUniform(const GLShader *shader) const
    {
      shader->applyUniform(mBuffer.name(), mBuffer.ptr(), (GLsizei)mBuffer.size(), mBuffer.type());
    }
    
    void applyStruct(const GLShader *shader) const
    {
      std::string name = mBuffer.name();
      for (const auto buffer : mBuffer)
        shader->applyUniform(name + "." + buffer.name(), buffer.ptr(), (GLsizei)buffer.size(), buffer.type());
    }
    
    Buffer mBuffer;
    GLuint mBufferId;
  };
}

#endif /* GLBuffer_h */
