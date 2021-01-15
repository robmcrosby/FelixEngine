//
//  MetalShaderProgram.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "GraphicResources.h"
#include "UniformMap.h"
#include "TextureMap.h"
#include <map>


#ifndef MetalShaderProgram_h
#define MetalShaderProgram_h

@protocol MTLDevice;
@protocol MTLFunction;
@protocol MTLRenderPipelineState;
@protocol MTLRenderCommandEncoder;

@class MTLRenderPipelineDescriptor;
@class MetalTextureSampler;


namespace fx {
  class MetalFrameBuffer;
  class Uniform;
  class BlendState;
  
  typedef std::map<int, id <MTLRenderPipelineState> > PipelineMap;
  typedef std::map<std::string, unsigned long> IndexMap;
  
  class MetalShaderProgram: public ShaderProgram {
  public:
    id <MTLDevice>   _device;
    id <MTLFunction> _vertex;
    id <MTLFunction> _fragment;
    
    PipelineMap _pipelines;
    IndexMap _attributeTypeMap;
    IndexMap _vertexIndexMap;
    IndexMap _fragmentIndexMap;
    IndexMap _textureIndexMap;
    IndexMap _samplerIndexMap;
    
  private:
    static unsigned int shaderCount;
    unsigned int _shaderId;
    
  public:
    MetalShaderProgram(id <MTLDevice> device);
    virtual ~MetalShaderProgram();
    
    virtual bool loadShaderFunctions(const std::string &vertex, const std::string &fragment);
    void encode(id <MTLRenderCommandEncoder> encoder, MetalFrameBuffer *frame, BlendState blendState);
    void encode(id <MTLRenderCommandEncoder> encoder, UniformsPtr uniforms);
    void encode(id <MTLRenderCommandEncoder> encoder, TexturesPtr textures, MetalTextureSampler *samplers);
    
    virtual unsigned int getShaderId() const;
    
  private:
    void addPipeline(MetalFrameBuffer *frame, const BlendState &blending);
    void setUniform(id <MTLRenderCommandEncoder> encoder, const std::string &name, const Uniform &uniform);
    void setTexture(id <MTLRenderCommandEncoder> encoder, const std::string &name, const Texture &texture, MetalTextureSampler *samplers);
    void setDescriptorAttributes(MTLRenderPipelineDescriptor *descriptor);
    void extractIndexMaps();
  };
}

#endif /* MetalShaderProgram_h */
