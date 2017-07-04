//
//  MetalFrameBuffer.mm
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "MetalFrameBuffer.h"
#include "MetalTextureBuffer.h"
#include "GraphicTask.h"

#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>

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
  _metalLayer = nil;
  _drawable = nil;
  _size = ivec2(0, 0);
}

MetalFrameBuffer::~MetalFrameBuffer() {
  
}

bool MetalFrameBuffer::resize(int width, int height) {
  if (_metalLayer == nil && (width != _size.w || height != _size.h)) {
    _size = ivec2(width, height);
    
    // TODO: Add Resizing of existing buffers
  }
  return true;
}

ivec2 MetalFrameBuffer::size() const {
  return _size;
}

bool MetalFrameBuffer::addDepthBuffer() {
  MTLTextureDescriptor *descriptor = [MTLTextureDescriptor new];
  descriptor.width = (NSUInteger)_size.w;
  descriptor.height = (NSUInteger)_size.h;
  descriptor.pixelFormat = MTLPixelFormatDepth32Float;
  
  _depthAttachment = [_device newTextureWithDescriptor:descriptor];
  return _depthAttachment != nil;
}

bool MetalFrameBuffer::addColorTexture() {
  MTLTextureDescriptor *descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                        width:(NSUInteger)_size.w
                                                                                       height:(NSUInteger)_size.h
                                                                                    mipmapped:NO];
  _colorAttachments.push_back([_device newTextureWithDescriptor:descriptor]);
  return _colorAttachments.back() != nil;
}

TextureBuffer* MetalFrameBuffer::getColorTexture(int index) {
  return new MetalTextureBuffer(_device, _colorAttachments.at(index));
}

id <MTLRenderCommandEncoder> MetalFrameBuffer::createEncoder(id<MTLCommandBuffer> buffer, const GraphicTask &task) {
  if (_metalLayer != nil && _drawable == nil)
    getNextDrawable();
  
  MTLRenderPassDescriptor *descriptor = [[MTLRenderPassDescriptor alloc] init];
  int index = 0;
  for (id <MTLTexture> colorAttachment : _colorAttachments) {
    MTLLoadAction loadAction = getMetalLoadAction(task.colorActions[index].loadAction);
    MTLStoreAction storeAction = getMetalStoreAction(task.colorActions[index].storeAction);
    vec4 color = task.colorActions[index].clearColor;
    
    descriptor.colorAttachments[index].texture     = colorAttachment;
    descriptor.colorAttachments[index].loadAction  = loadAction;
    descriptor.colorAttachments[index].storeAction = storeAction;
    descriptor.colorAttachments[index].clearColor  = MTLClearColorMake(color.r, color.g, color.b, color.a);
    ++index;
  }
  
  if (_depthAttachment != nil) {
    MTLLoadAction loadAction = getMetalLoadAction(task.depthStencilAction.loadAction);
    MTLStoreAction storeAction = getMetalStoreAction(task.depthStencilAction.storeAction);
    
    descriptor.depthAttachment.texture     = _depthAttachment;
    descriptor.depthAttachment.loadAction  = loadAction;
    descriptor.depthAttachment.storeAction = storeAction;
    descriptor.depthAttachment.clearDepth  = (double)task.depthStencilAction.clearColor.r;
  }
  
  if (_stencilAttachment != nil) {
    MTLLoadAction loadAction = getMetalLoadAction(task.depthStencilAction.loadAction);
    MTLStoreAction storeAction = getMetalStoreAction(task.depthStencilAction.storeAction);
    
    descriptor.stencilAttachment.texture      = _stencilAttachment;
    descriptor.stencilAttachment.loadAction   = loadAction;
    descriptor.stencilAttachment.storeAction  = storeAction;
    descriptor.stencilAttachment.clearStencil = (uint32_t)task.depthStencilAction.clearColor.g;
  }
  
  return [buffer renderCommandEncoderWithDescriptor:descriptor];
}

void MetalFrameBuffer::setMetalLayer(CAMetalLayer *layer) {
  _metalLayer = layer;
  
  _colorAttachments.push_back(nil);
  _size.w = (int)(_metalLayer.bounds.size.width * _metalLayer.contentsScale);
  _size.h = (int)(_metalLayer.bounds.size.height * _metalLayer.contentsScale);
}

void MetalFrameBuffer::present(id <MTLCommandBuffer> buffer) {
  if (_metalLayer != nil && _drawable != nil) {
    [buffer presentDrawable:_drawable];
    _colorAttachments.at(0) = nil;
    _drawable = nil;
  }
}

void MetalFrameBuffer::getNextDrawable() {
  _drawable = [_metalLayer nextDrawable];
  _colorAttachments.at(0) = _drawable.texture;
}
