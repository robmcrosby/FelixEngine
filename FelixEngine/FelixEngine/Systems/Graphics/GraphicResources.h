//
//  GraphicResources.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef GraphicResources_h
#define GraphicResources_h

#include <FelixEngine/Graphics.h>
#include <FelixEngine/iObject.h>
#include <FelixEngine/GraphicStates.h>
#include <FelixEngine/ImageBufferData.h>
#include <FelixEngine/XMLTree.h>
#include <FelixEngine/Vector.h>
#include <FelixEngine/Matrix.h>
#include <vector>


namespace fx {
  class VertexMeshData;
  class USDCrate;
  class USDItem;
  
  enum SHADER_PART {
    SHADER_VERTEX = 0,
    SHADER_FRAGMENT,
    SHADER_PART_COUNT,
  };
  
  enum VERTEX_PRIMITIVE {
    VERTEX_TRIANGLES,
    VERTEX_TRIANGLE_STRIP,
  };
  
  enum TEXTURE_FORMAT {
    TEXTURE_RGBA8,
    TEXTURE_RGBA16F,
    TEXTURE_DEPTH32F,
    TEXTURE_DEPTH32F_STENCIL8,
  };

  enum TEXTURE_USAGE_FLAGS {
    TEXTURE_SHADER_READ   = 1,
    TEXTURE_SHADER_WRITE  = 2,
    TEXTURE_RENDER_TARGET = 4,
  };
  
  struct ShaderProgram: iObject {
    virtual ~ShaderProgram() {}
    
    virtual void setScene(Scene *scene) {}
    virtual void update(float dt) {}
    virtual bool load(const XMLTree::Node &node);
    
    virtual bool loadShaderFunctions(const std::string &vertex, const std::string &fragment) = 0;
    
    virtual unsigned int getShaderId() const = 0;
  };
  
  struct VertexMesh: iObject {
    virtual ~VertexMesh() {}
    
    virtual void setScene(Scene *scene) {}
    virtual void update(float dt) {}
    virtual bool load(const XMLTree::Node &node);
    virtual bool load(const USDItem &item);
    
    virtual bool load(const VertexMeshData &data) = 0;
    virtual void setPrimativeType(VERTEX_PRIMITIVE type) = 0;
    virtual bool setIndexBuffer(size_t count, const int *buffer) = 0;
    virtual bool setVertexBuffer(const std::string &name, size_t size, size_t count, const float *buffer) = 0;
    virtual bool loadBuffers() = 0;
    
    bool setVertexBuffer(const std::string &name, const std::vector<float> &buffer);
    bool setVertexBuffer(const std::string &name, const std::vector<vec2> &buffer);
    bool setVertexBuffer(const std::string &name, const std::vector<vec4> &buffer);
    
    virtual unsigned int getMeshId() const = 0;
  };
  
  enum BUFFER_COUNT {
    BUFFER_SINGLE = 1,
    BUFFER_DOUBLE = 2,
    BUFFER_TRIPLE = 3,
  };
  
  struct UniformBuffer {
    virtual ~UniformBuffer() {}
    
    virtual bool load(void *data, size_t size, BUFFER_COUNT count) = 0;
    virtual void update(void *data, size_t size) = 0;
    
    template <typename T>
    T& operator=(T &data) {
      update(&data, sizeof(T));
      return data;
    }
    
    template <typename T>
    std::vector<T>& operator=(std::vector<T> &data) {
      update(&data.at(0), data.size()*sizeof(T));
      return data;
    }
  };
  
  struct TextureBuffer: iObject {
    virtual ~TextureBuffer() {}
    
    virtual void setScene(Scene *scene) {}
    virtual void update(float dt) {}
    virtual bool load(const XMLTree::Node &node);
    
    virtual bool loadBuffer(ivec2 size, TEXTURE_FORMAT format, int usageFlags) = 0;
    
    virtual bool loadImageFile(const std::string &file, bool generateMipMap);
    virtual bool loadColor(const RGBA &color);
    
    virtual bool loadCubeMapFile(const std::string &file, bool generateMipMap);
    virtual bool loadCubeMapFiles(const std::vector<std::string> &files, bool generateMipMap);
    virtual bool loadCubeColor(const RGBA &color);
    
    virtual bool loadImage(const ImageBufferData &image, bool generateMipMap) = 0;
    virtual bool loadCubeMap(const ImageBufferSet &images, bool generateMipMap) = 0;
    virtual bool loadCubeMap(const ImageBufferData &image, bool generateMipMap) = 0;
    
//    virtual void blitToTexture(TextureBufferPtr texture, int slice = 0, int level = 0) = 0;
//    virtual void generateMipMap() = 0;
    
    virtual bool loaded() const = 0;
    virtual ivec2 size() const = 0;
    
    virtual int usageFlags() const = 0;
    virtual TEXTURE_FORMAT format() const = 0;
    
    virtual void setDefaultSampler(SamplerState state) = 0;
    virtual SamplerState defaultSampler() const = 0;
  };
  
  struct FrameBuffer: iObject {
    virtual ~FrameBuffer() {}
    
    virtual void setScene(Scene *scene) {}
    virtual void update(float dt) {}
    virtual bool load(const XMLTree::Node &node);
    
    virtual bool setToWindow(int index) = 0;
    
    virtual bool resize(int width, int height) = 0;
    virtual ivec2 size() const = 0;
    
    TextureBufferPtr addRenderTarget(TEXTURE_FORMAT format, int usageFlags = 0);
    virtual void addRenderTarget(TextureBufferPtr texture) = 0;
    
    TextureBufferPtr addColorTexture();
    TextureBufferPtr addDepthBuffer();
    
    virtual TextureBufferPtr getColorTexture(int index) const = 0;
    virtual TextureBufferPtr getDepthTexture() const = 0;
  };
  
  struct BufferPool {
    virtual ~BufferPool() {}
  };

  struct CommandBuffer {
    virtual ~CommandBuffer() {}
    virtual void encodeRenderPass(RenderPassPtr pass) = 0;
    virtual void encodeBlitTexture(TextureBufferPtr src, TextureBufferPtr dst, int slice = 0, int level = 0) = 0;
    virtual void encodeGenerateMipmap(TextureBufferPtr texture) = 0;
    virtual void commit() = 0;
  };

}


#endif /* GraphicResources_h */
