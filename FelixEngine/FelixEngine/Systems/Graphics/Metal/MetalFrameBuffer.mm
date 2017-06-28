//
//  MetalFrameBuffer.mm
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "MetalFrameBuffer.h"
#include "GraphicTask.h"
#include <Metal/Metal.h>

using namespace fx;
using namespace std;


MTLLoadAction getMetalLoadAction(LOAD_ACTION action) {
  switch (action) {
    case LOAD_LAST:
      return MTLLoadActionLoad;
    case LOAD_NONE:
      return MTLLoadActionDontCare;
    case LOAD_CLEAR:
      return MTLLoadActionClear;
  }
}

MTLStoreAction getMetalStoreAction(STORE_ACTION action) {
  switch (action) {
    case fx::STORE_SAVE:
      return MTLStoreActionStore;
    case fx::STORE_DISCARD:
      return MTLStoreActionDontCare;
  }
}


MetalFrameBuffer::MetalFrameBuffer(id <MTLDevice> device): _device(device), _depthAttachment(nil), _stencilAttachment(nil) {
  _size = ivec2(0, 0);
  _scale = 1.0f;
}

MetalFrameBuffer::~MetalFrameBuffer() {
  
}

ivec2 MetalFrameBuffer::size() const {
  return _size;
}

float MetalFrameBuffer::scale() const {
  return _scale;
}

bool MetalFrameBuffer::addDepthBuffer() {
  MTLTextureDescriptor *descriptor = [[MTLTextureDescriptor alloc] init];
  descriptor.width = (NSUInteger)_size.w;
  descriptor.height = (NSUInteger)_size.h;
  descriptor.pixelFormat = MTLPixelFormatDepth32Float;
  
  _depthAttachment = [_device newTextureWithDescriptor:descriptor];
  return _depthAttachment != nil;
}

id <MTLRenderCommandEncoder> MetalFrameBuffer::createEncoder(id<MTLCommandBuffer> buffer, const GraphicTask &task) {
  MTLRenderPassDescriptor *descriptor = [[MTLRenderPassDescriptor alloc] init];
  
  int index = 0;
  for (id <MTLTexture> colorAttachment : _colorAttachments) {
    MTLLoadAction loadAction = getMetalLoadAction(task.colorAttachments[index].loadAction);
    MTLStoreAction storeAction = getMetalStoreAction(task.colorAttachments[index].storeAction);
    vec4 color = task.colorAttachments[index].clearColor;
    
    descriptor.colorAttachments[index].texture     = colorAttachment;
    descriptor.colorAttachments[index].loadAction  = loadAction;
    descriptor.colorAttachments[index].storeAction = storeAction;
    descriptor.colorAttachments[index].clearColor  = MTLClearColorMake(color.r, color.g, color.b, color.a);
    ++index;
  }
  
  if (_depthAttachment != nil) {
    MTLLoadAction loadAction = getMetalLoadAction(task.depthStencilAttachment.loadAction);
    MTLStoreAction storeAction = getMetalStoreAction(task.depthStencilAttachment.storeAction);
    
    descriptor.depthAttachment.texture     = _depthAttachment;
    descriptor.depthAttachment.loadAction  = loadAction;
    descriptor.depthAttachment.storeAction = storeAction;
    descriptor.depthAttachment.clearDepth  = (double)task.depthStencilAttachment.clearColor.r;
  }
  
  if (_stencilAttachment != nil) {
    MTLLoadAction loadAction = getMetalLoadAction(task.depthStencilAttachment.loadAction);
    MTLStoreAction storeAction = getMetalStoreAction(task.depthStencilAttachment.storeAction);
    
    descriptor.stencilAttachment.texture      = _stencilAttachment;
    descriptor.stencilAttachment.loadAction   = loadAction;
    descriptor.stencilAttachment.storeAction  = storeAction;
    descriptor.stencilAttachment.clearStencil = (uint32_t)task.depthStencilAttachment.clearColor.g;
  }
  
  return [buffer renderCommandEncoderWithDescriptor:descriptor];
}
