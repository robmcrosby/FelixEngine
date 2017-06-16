//
//  Graphics.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/14/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Graphics_h
#define Graphics_h

#include "GraphicResources.h"

namespace fx {
  
  /** File System */
  class Graphics {
  protected:
    static Graphics *instance;
    
  public:
    virtual ~Graphics() {}
    
    virtual FrameBuffer*   getMainWindowBuffer() = 0;
    virtual ShaderProgram* createShaderProgram() = 0;
    virtual VertexMesh*    createVertexMesh()    = 0;
  };
  
}

#endif /* Graphics_h */
