//
//  GraphicResources.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef GraphicResources_h
#define GraphicResources_h

#include "XMLTree.h"
#include "Vector.h"
#include "Matrix.h"
#include <vector>


namespace fx {
  class VertexMeshData;
  class ImageBufferData;
  
  enum SHADER_PART {
    SHADER_VERTEX,
    SHADER_FRAGMENT,
    SHADER_COUNT,
  };
  
  enum VERTEX_PRIMITIVE {
    VERTEX_TRIANGLES,
    VERTEX_TRIANGLE_STRIP,
  };
  
  struct ShaderProgram {
    virtual ~ShaderProgram() {}
    
    virtual bool loadXML(const XMLTree::Node &node);
    virtual bool loadShaderFunctions(const std::string &vertex, const std::string &fragment) = 0;
  };
  
  struct VertexMesh {
    virtual ~VertexMesh() {}
    
    virtual bool loadXML(const XMLTree::Node &node);
    virtual bool load(const VertexMeshData &data) = 0;
    virtual void setPrimativeType(VERTEX_PRIMITIVE type) = 0;
    virtual bool setIndexBuffer(size_t count, const int *buffer) = 0;
    virtual bool addVertexBuffer(const std::string &name, size_t size, size_t count, const float *buffer) = 0;
  };
  
  struct UniformBuffer {
    virtual ~UniformBuffer() {}
    
    virtual bool load(void *data, size_t size) = 0;
    virtual void update() = 0;
    
    template <typename T>
    T& operator=(T &data) {
      load(&data, sizeof(T));
      return data;
    }
    
    template <typename T>
    std::vector<T>& operator=(std::vector<T> &data) {
      load(&data.at(0), data.size()*sizeof(T));
      return data;
    }
  };
  
  struct TextureBuffer {
    virtual ~TextureBuffer() {}
    
    virtual bool load(const ImageBufferData &data) = 0;
    virtual bool loaded() const = 0;
    virtual ivec2 size() const = 0;
  };
  
  struct FrameBuffer {
    virtual ~FrameBuffer() {}
    
    virtual bool loadXML(const XMLTree::Node &node);
    virtual bool setToWindow(int index) = 0;
    
    virtual bool resize(int width, int height) = 0;
    virtual ivec2 size() const = 0;
    
    virtual bool addDepthBuffer() = 0;
    virtual bool addColorTexture() = 0;
    
    virtual TextureBuffer* getColorTexture(int index) = 0;
  };
}


#endif /* GraphicResources_h */
