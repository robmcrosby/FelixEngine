//
//  GLTexture.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GLTexture_h
#define GLTexture_h

#include "GLGraphicSystem.h"


namespace fx
{
  /**
   *
   */
  class GLTexture: public Texture
  {
  public:
    GLTexture(GLGraphicSystem *system);
    virtual ~GLTexture();
    
    virtual void reload();
    
    bool load();
    
  private:
    GLGraphicSystem *mGLSystem;
  };
}

#endif /* GLTexture_h */
