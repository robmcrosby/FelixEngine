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
    GLBuffer() {}
    virtual ~GLBuffer() {}
    
    void upload(const Buffer &buffer)
    {
      mBuffer = buffer;
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
        if (mBuffer.bufferType() == BUFFER_STRUCT)
          applyStruct(shader);
      }
    }
    
  private:
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
  };
}

#endif /* GLBuffer_h */
