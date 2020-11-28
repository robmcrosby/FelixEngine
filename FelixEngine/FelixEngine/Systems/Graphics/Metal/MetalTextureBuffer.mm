//
//  MetalTextureBuffer.m
//  FelixEngine
//
//  Created by Robert Crosby on 7/1/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "MetalTextureBuffer.h"
#include "ImageBufferData.h"
#include <Metal/Metal.h>

using namespace fx;
using namespace std;


MetalTextureBuffer::MetalTextureBuffer(id <MTLDevice> device, id <MTLCommandQueue> queue):
    _device(device), _queue(queue), _texture(nil), _loaded(false) {
  _width = 0;
  _height = 0;
  _mipLevels = 0;
}

MetalTextureBuffer::MetalTextureBuffer(id <MTLDevice> device, id <MTLCommandQueue> queue, id <MTLTexture> texture):
    _device(device), _queue(queue), _texture(texture), _loaded(false) {
  _width = (int)_texture.width;
  _height = (int)_texture.height;
  _mipLevels = 1;
}

MetalTextureBuffer::~MetalTextureBuffer() {
  _texture = nil;
}

bool MetalTextureBuffer::loadImage(const ImageBufferData &image, bool generateMipMap) {
  _width = image.size.width;
  _height = image.size.height;
  
  MTLTextureDescriptor *descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                        width:_width
                                                                                       height:_height
                                                                                    mipmapped:generateMipMap];
  [descriptor setUsage:MTLTextureUsageShaderRead];
  
  _mipLevels = (int)descriptor.mipmapLevelCount;
  
  _texture = [_device newTextureWithDescriptor:descriptor];
  if (_texture != nil) {
    MTLRegion region = MTLRegionMake2D(0, 0, _width, _height);
    NSUInteger bytesPerRow = _width * 4;
    [_texture replaceRegion:region mipmapLevel:0 withBytes:image.ptr() bytesPerRow:bytesPerRow];
    _loaded = !generateMipMap;
    if (generateMipMap)
      encodeGenerateMipMap();
    return true;
  }
  cerr << "Error Creating Metal Texture" << endl;
  return false;
}

bool MetalTextureBuffer::loadCubeMap(const ImageBufferSet &images) {
  _width = images.front().size.width;
  _height = images.front().size.height;
  MTLTextureDescriptor *descriptor = [MTLTextureDescriptor textureCubeDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                           size:_width
                                                                                      mipmapped:NO];
  [descriptor setUsage:MTLTextureUsageShaderRead];
  
  _texture = [_device newTextureWithDescriptor:descriptor];
  if (_texture != nil) {
    MTLRegion region = MTLRegionMake2D(0, 0, _width, _height);
    NSUInteger bytesPerRow = _width * 4;
    NSUInteger bytesPerImage = bytesPerRow * _height;
    for (int i = 0; i < images.size(); ++i)
      [_texture replaceRegion:region mipmapLevel:0 slice:i withBytes:images.at(i).ptr() bytesPerRow:bytesPerRow bytesPerImage:bytesPerImage];
    _loaded = true;
    return true;
  }
  cerr << "Error Creating Metal Cube Map" << endl;
  return false;
}

void MetalTextureBuffer::encodeGenerateMipMap() {
  id<MTLCommandBuffer> commandBuffer = [_queue commandBuffer];
  id<MTLBlitCommandEncoder> commandEncoder = [commandBuffer blitCommandEncoder];
  [commandEncoder generateMipmapsForTexture:_texture];
  [commandEncoder endEncoding];
  [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
    _loaded = true;
  }];
  [commandBuffer commit];
}

bool MetalTextureBuffer::loaded() const {
  return _loaded;
}

ivec2 MetalTextureBuffer::size() const {
  return ivec2(_width, _height);
}

void MetalTextureBuffer::encode(id <MTLRenderCommandEncoder> encoder, id <MTLSamplerState> sampler, unsigned long index) {
  [encoder setFragmentTexture:_texture atIndex:index];
  [encoder setFragmentSamplerState:sampler atIndex:index];
}

void MetalTextureBuffer::setMetalTexture(id <MTLTexture> texture) {
  _texture = texture;
  _width = (int)_texture.width;
  _height = (int)_texture.height;
}
