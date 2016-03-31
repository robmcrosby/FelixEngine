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
    
    void upload(const Buffer &bufferMap)
    {
      mBuffers.resize(bufferMap.mapSize());
      
      int i = 0;
      for (const Buffer buffer : bufferMap)
        mBuffers[i++].upload(buffer);
    }
    
    void unload()
    {
      for (GLBuffer &buffer : mBuffers)
        buffer.unload();
    }
    
    void applyToShader(const GLShader *shader)
    {
      if (shader)
      {
        for (GLBuffer &buffer : mBuffers)
          buffer.applyToShader(shader);
      }
    }
    
  private:
    std::vector<GLBuffer> mBuffers;
  };
}


#endif /* GLUniforms_h */
