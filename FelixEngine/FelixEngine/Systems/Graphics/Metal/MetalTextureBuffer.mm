//
//  MetalTextureBuffer.m
//  FelixEngine
//
//  Created by Robert Crosby on 7/1/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "MetalTextureBuffer.h"
#include "ImageBufferData.h"
#include "MetalTextureSampler.h"
#include "Quaternion.h"
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

bool MetalTextureBuffer::loadCubeMap(const ImageBufferSet &images, bool generateMipMap) {
  _width = _height = images.front().size.height;
  MTLTextureDescriptor *descriptor = [MTLTextureDescriptor textureCubeDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                           size:_width
                                                                                      mipmapped:generateMipMap];
  [descriptor setUsage:MTLTextureUsageShaderRead];
  _mipLevels = (int)descriptor.mipmapLevelCount;
  _texture = [_device newTextureWithDescriptor:descriptor];
  if (_texture != nil) {
    MTLRegion region = MTLRegionMake2D(0, 0, _width, _height);
    NSUInteger bytesPerRow = _width * 4;
    NSUInteger bytesPerImage = bytesPerRow * _height;
    for (int i = 0; i < images.size(); ++i)
      [_texture replaceRegion:region mipmapLevel:0 slice:i withBytes:images.at(i).ptr() bytesPerRow:bytesPerRow bytesPerImage:bytesPerImage];
    _loaded = !generateMipMap;
    if (generateMipMap)
      encodeGenerateMipMap();
    return true;
  }
  cerr << "Error Creating Metal Cube Map" << endl;
  return false;
}

bool MetalTextureBuffer::loadCubeMap(const ImageBufferData &image, bool generateMipMap) {
  _width = _height = image.size.height/2;
  MTLTextureDescriptor *descriptor = [MTLTextureDescriptor textureCubeDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                           size:_width
                                                                                      mipmapped:generateMipMap];
  [descriptor setUsage:MTLTextureUsageShaderRead];
  _mipLevels = (int)descriptor.mipmapLevelCount;
  _texture = [_device newTextureWithDescriptor:descriptor];
  encodeGenerateCubeMap(image, generateMipMap);
  return true;
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

void MetalTextureBuffer::encodeGenerateMipMap() {
  id<MTLCommandBuffer> commandBuffer = [_queue commandBuffer];
  id<MTLBlitCommandEncoder> blitEncoder = [commandBuffer blitCommandEncoder];
  [blitEncoder generateMipmapsForTexture:_texture];
  [blitEncoder endEncoding];
  [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
    _loaded = true;
  }];
  [commandBuffer commit];
}

void MetalTextureBuffer::encodeGenerateCubeMap(const ImageBufferData &srcImage, bool generateMipMap) {
  // Setup Render Target
  MTLTextureDescriptor *descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                        width:_width
                                                                                       height:_height
                                                                                    mipmapped:NO];
  descriptor.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
  id <MTLTexture> renderTarget = [_device newTextureWithDescriptor:descriptor];
  
  // Setup the Source Texture
  descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                        width:srcImage.size.width
                                                                                       height:srcImage.size.height
                                                                                    mipmapped:NO];
  [descriptor setUsage:MTLTextureUsageShaderRead];
  id <MTLTexture> srcTexture = [_device newTextureWithDescriptor:descriptor];
  MTLRegion region = MTLRegionMake2D(0, 0, srcImage.size.width, srcImage.size.height);
  NSUInteger bytesPerRow = srcImage.size.width * 4;
  [srcTexture replaceRegion:region mipmapLevel:0 withBytes:srcImage.ptr() bytesPerRow:bytesPerRow];
  
  // Setup Pipeline Descriptor
  MTLRenderPipelineDescriptor *pipelineDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
  pipelineDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatRGBA8Unorm;
  pipelineDescriptor.vertexDescriptor.attributes[0].bufferIndex = 0;
  pipelineDescriptor.vertexDescriptor.attributes[0].offset = 0;
  pipelineDescriptor.vertexDescriptor.attributes[0].format = MTLVertexFormatFloat2;
  pipelineDescriptor.vertexDescriptor.layouts[0].stride = 2*sizeof(float);
  
  // Add Shader Functions to Pipeline Descriptor
  NSBundle *bundle = [NSBundle bundleForClass:[MetalTextureSampler class]];
  NSString *path = [bundle pathForResource:@"default" ofType:@"metallib"];
  id <MTLLibrary> library = [_device newLibraryWithFile:path error:nil];
  pipelineDescriptor.vertexFunction = [library newFunctionWithName:@"v_cube_gen"];
  pipelineDescriptor.fragmentFunction = [library newFunctionWithName:@"f_cube_gen"];
  id <MTLRenderPipelineState> pipeline = [_device newRenderPipelineStateWithDescriptor:pipelineDescriptor error:nil];
  
  // Setup Render Descriptor
  MTLRenderPassDescriptor *renderDescriptor = [[MTLRenderPassDescriptor alloc] init];
  renderDescriptor.colorAttachments[0].texture = renderTarget;
  renderDescriptor.colorAttachments[0].loadAction = MTLLoadActionDontCare;
  renderDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
  
  // Setup Rotations
  vector<quat> rotationData;
  rotationData.push_back(quat::RotZ(M_PI/2.0f));  // Right
  rotationData.push_back(quat::RotZ(-M_PI/2.0f)); // Left
  rotationData.push_back(quat::RotY(-M_PI/2.0f)); // Up
  rotationData.push_back(quat::RotY(M_PI/2.0f));  // Down
  rotationData.push_back(quat::RotZ(M_PI));       // Back
  rotationData.push_back(quat::RotZ(0.0f));       // Front
  id <MTLBuffer> rotations = [_device newBufferWithLength:6*sizeof(quat) options:MTLResourceCPUCacheModeDefaultCache];
  memcpy(rotations.contents, &rotationData[0], 6*sizeof(quat));
  
  // Setup Cube Gen Vertex Buffer
  float buffer[] = {-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f};
  id <MTLBuffer> vertBuffer = [_device newBufferWithBytes:buffer length:8*sizeof(float) options:MTLResourceCPUCacheModeDefaultCache];
  
  // Crate the Render Encoder
  id<MTLCommandBuffer> commandBuffer = [_queue commandBuffer];
  
  for (int i = 0; i < 6; ++i) {
    id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderDescriptor];
    [renderEncoder setRenderPipelineState:pipeline];
    [renderEncoder setFragmentBuffer:rotations offset:i*sizeof(quat) atIndex:0];
    [renderEncoder setFragmentTexture:srcTexture atIndex:0];
    [renderEncoder setVertexBuffer:vertBuffer offset:0 atIndex:0];
    [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangleStrip vertexStart:0 vertexCount:4 instanceCount:1];
    [renderEncoder endEncoding];
    
    id<MTLBlitCommandEncoder> blitEncoder = [commandBuffer blitCommandEncoder];
    [blitEncoder copyFromTexture:renderTarget sourceSlice:0 sourceLevel:0 toTexture:_texture destinationSlice:i destinationLevel:0 sliceCount:1 levelCount:1];
    [blitEncoder endEncoding];
  }
  if (generateMipMap) {
    id<MTLBlitCommandEncoder> blitEncoder = [commandBuffer blitCommandEncoder];
    [blitEncoder generateMipmapsForTexture:_texture];
    [blitEncoder endEncoding];
  }
  
  [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
    _loaded = true;
  }];
  [commandBuffer commit];
}
