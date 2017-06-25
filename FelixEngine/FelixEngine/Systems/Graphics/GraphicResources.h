//
//  GraphicResources.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef GraphicResources_h
#define GraphicResources_h

#include "Vector.h"
#include "Matrix.h"


namespace fx {
  class VertexMeshData;
  
  enum SHADER_PART {
    SHADER_VERTEX,
    SHADER_FRAGMENT,
    SHADER_COUNT,
  };
  
  enum VERTEX_PRIMITIVE {
    VERTEX_TRIANGLES,
    VERTEX_TRIANGLE_STRIP,
  };
  
  struct FrameBuffer {
    virtual ~FrameBuffer() {}
    
    virtual ivec2 size() const = 0;
    virtual float scale() const = 0;
  };
  
  struct ShaderProgram {
    virtual ~ShaderProgram() {}
    
    virtual bool loadShaderFunctions(const std::string &vertex, const std::string &fragment) = 0;
  };
  
  struct VertexMesh {
    virtual ~VertexMesh() {}
    
    virtual bool loadData(const VertexMeshData &data) = 0;
    virtual void setPrimativeType(VERTEX_PRIMITIVE type) = 0;
    virtual bool setIndexBuffer(int count, int *buffer) = 0;
    virtual bool addVertexBuffer(int size, int count, float *buffer) = 0;
  };
}


#endif /* GraphicResources_h */
