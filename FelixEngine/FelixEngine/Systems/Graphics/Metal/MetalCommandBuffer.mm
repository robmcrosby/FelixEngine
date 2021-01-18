//
//  MetalCommandBuffer.m
//  FelixEngine
//
//  Created by Robert Crosby on 1/18/21.
//  Copyright © 2021 Robert Crosby. All rights reserved.
//

#include "MetalCommandBuffer.h"
#include "MetalRenderPass.h"
#include "MetalTextureBuffer.h"
#include <Metal/Metal.h>


using namespace fx;
using namespace std;

MetalCommandBuffer::MetalCommandBuffer(id <MTLCommandBuffer> buffer) {
  _buffer = buffer;
}

MetalCommandBuffer::~MetalCommandBuffer() {
  commit();
}

void MetalCommandBuffer::encodeRenderPass(RenderPassPtr pass) {
  if (_buffer != nil) {
    MetalRenderPassPtr renderPass = static_pointer_cast<MetalRenderPass>(pass);
    renderPass->encodeRender(_buffer);
  }
}

void MetalCommandBuffer::encodeBlitTexture(TextureBuffer *src, TextureBuffer *dst, int slice, int level) {
  if (_buffer != nil) {
//    MetalTextureBufferPtr srcTexture = static_pointer_cast<MetalTextureBuffer>(src);
//    MetalTextureBufferPtr dstTexture = static_pointer_cast<MetalTextureBuffer>(dst);
    MetalTextureBuffer *srcTexture = (MetalTextureBuffer*)src;
    MetalTextureBuffer *dstTexture = (MetalTextureBuffer*)dst;
    srcTexture->encodeBlitToTexture(_buffer, dstTexture->_texture, slice, level);
  }
}

void MetalCommandBuffer::encodeGenerateMipmap(TextureBuffer *texture) {
  if (_buffer != nil) {
    //MetalTextureBufferPtr mtlTexture = static_pointer_cast<MetalTextureBuffer>(texture);
    MetalTextureBuffer *mtlTexture = (MetalTextureBuffer*)texture;
    mtlTexture->encodeGenerateMipMap(_buffer);
  }
}

void MetalCommandBuffer::commit() {
  if (_buffer != nil) {
    [_buffer commit];
    _buffer = nil;
  }
}
