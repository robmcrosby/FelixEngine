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
    
    bool load();
    
    virtual void setVertexBufferMap(const VertexBufferMap &map);
    virtual void addVertexBuffer(const std::string &name, int components, int count, const float *data);
    virtual void setIndexBuffer(int count, const int *data);
    virtual void setPrimitiveType(VERTEX_PRIMITIVE type);
    
  private:
    GLGraphicSystem *mGLSystem;
    VertexBufferMap mVertexBuffMap;
  };
}

#endif /* GLMesh_h */
