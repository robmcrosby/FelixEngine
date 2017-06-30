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
    virtual bool addDepthBuffer() = 0;
  };
  
  struct ShaderProgram {
    virtual ~ShaderProgram() {}
    
    virtual bool loadShaderFunctions(const std::string &vertex, const std::string &fragment) = 0;
  };
  
  struct VertexMesh {
    virtual ~VertexMesh() {}
    
    virtual bool load(const VertexMeshData &data) = 0;
    virtual void setPrimativeType(VERTEX_PRIMITIVE type) = 0;
    virtual bool setIndexBuffer(size_t count, const int *buffer) = 0;
    virtual bool addVertexBuffer(const std::string &name, size_t size, size_t count, const float *buffer) = 0;
  };
  
  struct UniformBuffer {
    virtual ~UniformBuffer() {}
    
    virtual bool load(const void *data, size_t size) = 0;
    virtual bool update(const void *data, size_t size) = 0;
    
    template <typename T>
    const T& operator=(const T &data) {
      load(&data, sizeof(T));
      return data;
    }
  };
}


#endif /* GraphicResources_h */