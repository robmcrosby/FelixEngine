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
    
    void uploadBuffer(const Buffer &buffer)
    {
      
    }
    
    void applyToShader(const GLShader *shader)
    {
      if (shader)
      {
      }
    }
    
  };
}

#endif /* GLBuffer_h */
