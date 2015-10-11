//
//  GLMesh.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GLMesh_h
#define GLMesh_h

#include "GraphicSystem.h"


namespace fx
{
  class GLGraphicSystem;
  
  /**
   *
   */
  class GLMesh: public Mesh
  {
  public:
    GLMesh(GLGraphicSystem *system);
    virtual ~GLMesh();
    
    virtual bool load();
    
    virtual bool addVertexBuffer(const std::string &name, int size, int count, const float *data);
    virtual bool setIndexBuffer(int count, const int *data);
    virtual void setPrimitiveType(VERTEX_PRIMITIVE type);
    
  private:
    GLGraphicSystem *mGLSystem;
  };
}

#endif /* GLMesh_h */
