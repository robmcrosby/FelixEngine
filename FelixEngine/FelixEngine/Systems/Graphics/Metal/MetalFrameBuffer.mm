//
//  MetalFrameBuffer.mm
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "MetalFrameBuffer.h"
#include "MetalTextureBuffer.h"
#include "MetalGraphics.h"

#include <Metal/Metal.h>
#include <UIKit/UIKit.h>
#include <QuartzCore/CAMetalLayer.h>

#define ID_FLAG_OFFSET 24

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

MetalFrameBuffer::MetalFrameBuffer(id <MTLDevice> device, id <MTLCommandQueue> queue): _device(device), _queue(queue) {
  static int bufferCount = 1;
  _idFlag = bufferCount++ << ID_FLAG_OFFSET;
  
  _metalLayer = nil;
  _drawable = nil;
  
  CGSize size = [[UIScreen mainScreen] nativeBounds].size;
  _size = ivec2((int)size.width, (int)size.height);
}

MetalFrameBuffer::~MetalFrameBuffer() {
  
}

bool MetalFrameBuffer::setToWindow(int index) {
  MetalGraphics &graphics = static_cast<MetalGraphics&>(Graphics::getInstance());
  return graphics.setWindowBuffer(this, index);
}

bool MetalFrameBuffer::resize(int width, int height) {
  bool success = true;
  
  if (_metalLayer == nil && (width != _size.w || height != _size.h)) {
    _size = ivec2(width, height);
    
//    // Resize the Color Attachments
//    for (auto attachment : _colorAttachments) {
//      attachment = resizeTexture(attachment, _size);
//      success &= attachment != nil;
//    }
//
//    // Resize the Depth Atacment
//    if (_depthAttachment != nil) {
//      _depthAttachment = resizeTexture(_depthAttachment, _size);
//      success &= _depthAttachment != nil;
//    }
//
//    // Resize the Stencil Atacment
//    if (_stencilAttachment != nil) {
//      _stencilAttachment = resizeTexture(_stencilAttachment, _size);
//      success &= _stencilAttachment != nil;
//    }
  }
  return success;
}

id <MTLTexture> MetalFrameBuffer::resizeTexture(id <MTLTexture> texture, ivec2 size) {
  MTLTextureDescriptor *descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:texture.pixelFormat
                                                                                        width:size.w
                                                                                       height:size.h
                                                                                    mipmapped:NO];
  descriptor.usage = texture.usage;
  return [_device newTextureWithDescriptor:descriptor];
}

ivec2 MetalFrameBuffer::size() const {
  return _size;
}

void MetalFrameBuffer::addRenderTarget(TextureBufferPtr texture) {
  if (texture->format() == TEXTURE_DEPTH32F || texture->format() == TEXTURE_DEPTH32F)
    _depthAttachment = static_pointer_cast<MetalTextureBuffer>(texture);
  else
    _colorAttachments.push_back(static_pointer_cast<MetalTextureBuffer>(texture));
}

TextureBufferPtr MetalFrameBuffer::getColorTexture(int index) const {
  return _colorAttachments.at(index);
}

TextureBufferPtr MetalFrameBuffer::getDepthTexture() const {
  return _depthAttachment;
}

int MetalFrameBuffer::pipelineId(const BlendState &blending) const {
  return _idFlag | blending.flags;
}

id <MTLRenderCommandEncoder> MetalFrameBuffer::createEncoder(id<MTLCommandBuffer> buffer, const ActionState &depthStencilAction, const ActionState *colorActions) {
  if (_metalLayer != nil && _drawable == nil)
    getNextDrawable();
  
  MTLRenderPassDescriptor *descriptor = [[MTLRenderPassDescriptor alloc] init];
  int index = 0;
  for (MetalTextureBufferPtr attachment : _colorAttachments) {
    MTLLoadAction loadAction = getMetalLoadAction(colorActions[index].loadAction);
    MTLStoreAction storeAction = getMetalStoreAction(colorActions[index].storeAction);
    vec4 color = colorActions[index].clearColor;
    
    descriptor.colorAttachments[index].texture     = attachment->getMetalTexture();
    descriptor.colorAttachments[index].loadAction  = loadAction;
    descriptor.colorAttachments[index].storeAction = storeAction;
    descriptor.colorAttachments[index].clearColor  = MTLClearColorMake(color.r, color.g, color.b, color.a);
    ++index;
  }
  
  if (_depthAttachment) {
    MTLLoadAction loadAction = getMetalLoadAction(depthStencilAction.loadAction);
    MTLStoreAction storeAction = getMetalStoreAction(depthStencilAction.storeAction);
    
    descriptor.depthAttachment.texture     = _depthAttachment->getMetalTexture();
    descriptor.depthAttachment.loadAction  = loadAction;
    descriptor.depthAttachment.storeAction = storeAction;
    descriptor.depthAttachment.clearDepth  = (double)depthStencilAction.clearColor.r;
  }
  
//  if (_stencilAttachment != nil) {
//    MTLLoadAction loadAction = getMetalLoadAction(depthStencilAction.loadAction);
//    MTLStoreAction storeAction = getMetalStoreAction(depthStencilAction.storeAction);
//
//    descriptor.stencilAttachment.texture      = _stencilAttachment;
//    descriptor.stencilAttachment.loadAction   = loadAction;
//    descriptor.stencilAttachment.storeAction  = storeAction;
//    descriptor.stencilAttachment.clearStencil = (uint32_t)depthStencilAction.clearColor.g;
//  }
  
  return [buffer renderCommandEncoderWithDescriptor:descriptor];
}

void MetalFrameBuffer::setMetalLayer(CAMetalLayer *layer) {
  _metalLayer = layer;
  _size.w = (int)(_metalLayer.bounds.size.width * _metalLayer.contentsScale);
  _size.h = (int)(_metalLayer.bounds.size.height * _metalLayer.contentsScale);
  
  if (_colorAttachments.size() == 0) {
    MetalTextureBufferPtr texture = make_shared<MetalTextureBuffer>(_device, _queue);
    _colorAttachments.push_back(texture);
  }
}

void MetalFrameBuffer::present(id <MTLCommandBuffer> buffer) {
  if (_metalLayer != nil && _drawable != nil) {
    [buffer presentDrawable:_drawable];
    _colorAttachments.front()->setMetalTexture(nil);
    _drawable = nil;
  }
}

MTLRenderPipelineDescriptor* MetalFrameBuffer::createPipelineDescriptor(const BlendState &blending) const {
  MTLRenderPipelineDescriptor *descriptor = [[MTLRenderPipelineDescriptor alloc] init];
  
  int index = 0;
  for (MetalTextureBufferPtr attachment : _colorAttachments) {
    MTLRenderPipelineColorAttachmentDescriptor *color = descriptor.colorAttachments[index];
    color.pixelFormat = attachment->getMetalTexture().pixelFormat;
    if (blending.enabled()) {
      color.blendingEnabled = YES;
      color.rgbBlendOperation = (MTLBlendOperation)metalBlendOperation(blending.equation());
      color.sourceRGBBlendFactor = (MTLBlendFactor)metalBlendFactor(blending.src());
      color.destinationRGBBlendFactor = (MTLBlendFactor)metalBlendFactor(blending.dst());
      if (blending.seperate()) {
        color.alphaBlendOperation = (MTLBlendOperation)metalBlendOperation(blending.equationAlpha());
        color.sourceAlphaBlendFactor = (MTLBlendFactor)metalBlendFactor(blending.srcAlpha());
        color.destinationAlphaBlendFactor = (MTLBlendFactor)metalBlendFactor(blending.dstAlpha());
      }
    }
  }
  
  if (_depthAttachment) {
    descriptor.depthAttachmentPixelFormat = _depthAttachment->getMetalTexture().pixelFormat;
  }
//  if (_stencilAttachment != nil) {
//    descriptor.stencilAttachmentPixelFormat = _stencilAttachment.pixelFormat;
//  }
  
  return descriptor;
}

void MetalFrameBuffer::getNextDrawable() {
  _size.w = _metalLayer.bounds.size.width;
  _size.h = _metalLayer.bounds.size.height;
  
  _drawable = [_metalLayer nextDrawable];
  _colorAttachments.front()->setMetalTexture(_drawable.texture);
}

int MetalFrameBuffer::metalBlendFactor(int factor) const {
  if (factor == BLEND_INPUT_SRC_ALPHA_SATURATE)
    return MTLBlendFactorSourceAlphaSaturated;
  if (factor & BLEND_INPUT_ONE_MINUS)
  {
    factor &= ~BLEND_INPUT_ONE_MINUS;
    switch (factor)
    {
      case BLEND_INPUT_SRC_COLOR:   return MTLBlendFactorOneMinusSourceColor;
      case BLEND_INPUT_SRC_ALPHA:   return MTLBlendFactorOneMinusSourceAlpha;
      case BLEND_INPUT_DST_COLOR:   return MTLBlendFactorOneMinusDestinationColor;
      case BLEND_INPUT_DST_ALPHA:   return MTLBlendFactorOneMinusDestinationAlpha;
      case BLEND_INPUT_CONST_COLOR: return MTLBlendFactorOneMinusBlendColor;
      case BLEND_INPUT_CONST_ALPHA: return MTLBlendFactorOneMinusBlendAlpha;
    }
  }
  else
  {
    switch (factor)
    {
      case BLEND_INPUT_SRC_COLOR:   return MTLBlendFactorSourceColor;
      case BLEND_INPUT_SRC_ALPHA:   return MTLBlendFactorSourceAlpha;
      case BLEND_INPUT_DST_COLOR:   return MTLBlendFactorDestinationColor;
      case BLEND_INPUT_DST_ALPHA:   return MTLBlendFactorDestinationAlpha;
      case BLEND_INPUT_CONST_COLOR: return MTLBlendFactorBlendColor;
      case BLEND_INPUT_CONST_ALPHA: return MTLBlendFactorBlendAlpha;
      case BLEND_INPUT_ZERO:        return MTLBlendFactorZero;
    }
  }
  return MTLBlendFactorOne;
}

int MetalFrameBuffer::metalBlendOperation(int operation) const {
  switch (operation)
  {
    case BLEND_EQ_ADD:          return MTLBlendOperationAdd;
    case BLEND_EQ_SUBTRACT:     return MTLBlendOperationSubtract;
    case BLEND_EQ_REV_SUBTRACT: return MTLBlendOperationReverseSubtract;
    case BLEND_EQ_MIN:          return MTLBlendOperationMin;
    case BLEND_EQ_MAX:          return MTLBlendOperationMax;
  }
  return MTLBlendOperationAdd;
}
