//
//  MetalShaderProgram.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "GraphicResources.h"
#include <map>

@protocol MTLDevice;
@protocol MTLFunction;
@protocol MTLRenderPipelineState;
@protocol MTLRenderCommandEncoder;

namespace fx {
  class MetalFrameBuffer;
  class GraphicTask;
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
    IndexMap _vertexIndexMap;
    IndexMap _fragmentIndexMap;
    
  public:
    MetalShaderProgram(id <MTLDevice> device);
    virtual ~MetalShaderProgram();
    
    virtual bool loadShaderFunctions(const std::string &vertex, const std::string &fragment);
    void encode(id <MTLRenderCommandEncoder> encoder, const GraphicTask &task);
    
  private:
    void addPipeline(MetalFrameBuffer *frame, const BlendState &blending);
    void setUniform(id <MTLRenderCommandEncoder> encoder, const std::string &name, const Uniform &uniform);
    void extractIndexMaps();
  };
}
