//
//  MetalUniformBuffer.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/26/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "GraphicResources.h"
#include <map>

#define MAX_BUFFERS 3

@protocol MTLDevice;
@protocol MTLBuffer;
@protocol MTLRenderCommandEncoder;


namespace fx {
  class MetalShaderProgram;
  
  class MetalUniformBuffer: public UniformBuffer {
  public:
    id <MTLDevice> _device;
    id <MTLBuffer> _buffers[MAX_BUFFERS];
    
    size_t _bufferSize;
    int _currentBuffer;
    int _lastBuffer;
    int _bufferCount;
    
  private:
    void clearBuffers();
    
  public:
    MetalUniformBuffer(id <MTLDevice> device);
    virtual ~MetalUniformBuffer();
    
    virtual bool load(void *data, size_t size, BUFFER_COUNT count);
    virtual void update(void *data, size_t size);
    
    void encodeCurrent(id <MTLRenderCommandEncoder> encoder, const std::string &name, MetalShaderProgram *shader);
    void encodeLast(id <MTLRenderCommandEncoder> encoder, const std::string &name, MetalShaderProgram *shader);
    void encode(id <MTLRenderCommandEncoder> encoder, const std::string &name, MetalShaderProgram *shader, int buffer);
  };
}
