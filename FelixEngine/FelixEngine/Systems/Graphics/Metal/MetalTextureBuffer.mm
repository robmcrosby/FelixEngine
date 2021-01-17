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


MTLPixelFormat getMetalPixelFormat(TEXTURE_FORMAT format) {
  switch (format) {
    case TEXTURE_RGBA8:
      return MTLPixelFormatRGBA8Unorm;
    case TEXTURE_RGBA16F:
      return MTLPixelFormatRGBA16Float;
    case TEXTURE_DEPTH32F:
      return MTLPixelFormatDepth32Float;
    case TEXTURE_DEPTH32F_STENCIL8:
      return MTLPixelFormatDepth32Float_Stencil8;
  }
  cerr << "Unknown Texture Format: " << format << endl;
  return MTLPixelFormatRGBA8Unorm;
}


MetalTextureBuffer::MetalTextureBuffer(id <MTLDevice> device, id <MTLCommandQueue> queue):
    _device(device), _queue(queue), _texture(nil), _loaded(false) {
}

MetalTextureBuffer::MetalTextureBuffer(id <MTLDevice> device, id <MTLCommandQueue> queue, id <MTLTexture> texture):
    _device(device), _queue(queue), _texture(texture), _loaded(false) {
}

MetalTextureBuffer::~MetalTextureBuffer() {
  _texture = nil;
}

bool MetalTextureBuffer::loadBuffer(ivec2 size, TEXTURE_FORMAT format, int usageFlags) {
  MTLPixelFormat pixelFormat = getMetalPixelFormat(format);
  MTLTextureDescriptor *descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:pixelFormat
                                                                                        width:size.width
                                                                                       height:size.height
                                                                                    mipmapped:NO];
  descriptor.usage |= usageFlags & TEXTURE_SHADER_READ ? MTLTextureUsageShaderRead : 0;
  descriptor.usage |= usageFlags & TEXTURE_SHADER_WRITE ? MTLTextureUsageShaderWrite : 0;
  descriptor.usage |= usageFlags & TEXTURE_RENDER_TARGET ? MTLTextureUsageRenderTarget : 0;
  
  _texture = [_device newTextureWithDescriptor:descriptor];
  return _loaded = _texture != nil;
}

bool MetalTextureBuffer::loadImage(const ImageBufferData &image, bool generateMipMap) {
  MTLPixelFormat pixelFormat = getMetalPixelFormat(image.pixelBytes == 4 ? TEXTURE_RGBA8 : TEXTURE_RGBA16F);
  MTLTextureDescriptor *descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:pixelFormat
                                                                                        width:image.size.width
                                                                                       height:image.size.height
                                                                                    mipmapped:generateMipMap];
  descriptor.usage = MTLTextureUsageShaderRead;
  
  _texture = [_device newTextureWithDescriptor:descriptor];
  if (_texture != nil) {
    MTLRegion region = MTLRegionMake2D(0, 0, image.size.width, image.size.height);
    NSUInteger bytesPerRow = image.size.width * image.pixelBytes;
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
  MTLPixelFormat pixelFormat = getMetalPixelFormat(images.front().pixelBytes == 4 ? TEXTURE_RGBA8 : TEXTURE_RGBA16F);
  int size = images.front().size.height;
  MTLTextureDescriptor *descriptor = [MTLTextureDescriptor textureCubeDescriptorWithPixelFormat:pixelFormat
                                                                                           size:size
                                                                                      mipmapped:generateMipMap];
  [descriptor setUsage:MTLTextureUsageShaderRead];
  
  _texture = [_device newTextureWithDescriptor:descriptor];
  if (_texture != nil) {
    MTLRegion region = MTLRegionMake2D(0, 0, size, size);
    NSUInteger bytesPerRow = size * images.front().pixelBytes;
    NSUInteger bytesPerImage = bytesPerRow * size;
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
  MTLPixelFormat pixelFormat = getMetalPixelFormat(image.pixelBytes == 4 ? TEXTURE_RGBA8 : TEXTURE_RGBA16F);
  int size = image.size.height/2;
  MTLTextureDescriptor *descriptor = [MTLTextureDescriptor textureCubeDescriptorWithPixelFormat:pixelFormat
                                                                                           size:size
                                                                                      mipmapped:generateMipMap];
  [descriptor setUsage:MTLTextureUsageShaderRead];
  _texture = [_device newTextureWithDescriptor:descriptor];
  encodeGenerateCubeMap(image, generateMipMap);
  return true;
}

bool MetalTextureBuffer::loaded() const {
  return _loaded;
}

ivec2 MetalTextureBuffer::size() const {
  if (_texture != nil)
    return ivec2((int)_texture.width, (int)_texture.height);
  return ivec2(0, 0);
}

int MetalTextureBuffer::usageFlags() const {
  int flags = 0;
  if (_texture != nil) {
    flags = _texture.usage & MTLTextureUsageShaderRead ? TEXTURE_SHADER_READ : 0;
    flags = _texture.usage & MTLTextureUsageShaderWrite ? TEXTURE_SHADER_WRITE : 0;
    flags = _texture.usage & MTLTextureUsageRenderTarget ? TEXTURE_RENDER_TARGET : 0;
  }
  return flags;
}

TEXTURE_FORMAT MetalTextureBuffer::format() const {
  if (_texture != nil) {
    if (_texture.pixelFormat == MTLPixelFormatRGBA8Unorm)
      return TEXTURE_RGBA8;
    if (_texture.pixelFormat == MTLPixelFormatRGBA16Float)
      return TEXTURE_RGBA16F;
    if (_texture.pixelFormat == MTLPixelFormatDepth32Float)
      return TEXTURE_DEPTH32F;
    if (_texture.pixelFormat == MTLPixelFormatDepth32Float_Stencil8)
      return TEXTURE_DEPTH32F_STENCIL8;
  }
  return TEXTURE_RGBA8;
}

void MetalTextureBuffer::encode(id <MTLRenderCommandEncoder> encoder, id <MTLSamplerState> sampler, unsigned long index) {
  if (_texture) {
    [encoder setFragmentTexture:_texture atIndex:index];
    [encoder setFragmentSamplerState:sampler atIndex:index];
  }
}

void MetalTextureBuffer::setMetalTexture(id <MTLTexture> texture) {
  _texture = texture;
  _loaded = texture != nil;
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
  MTLTextureDescriptor *descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:_texture.pixelFormat
                                                                                        width:_texture.width
                                                                                       height:_texture.height
                                                                                    mipmapped:NO];
  descriptor.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
  id <MTLTexture> renderTarget = [_device newTextureWithDescriptor:descriptor];
  
  // Setup the Source Texture
  descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:_texture.pixelFormat
                                                                  width:srcImage.size.width
                                                                 height:srcImage.size.height
                                                              mipmapped:NO];
  [descriptor setUsage:MTLTextureUsageShaderRead];
  id <MTLTexture> srcTexture = [_device newTextureWithDescriptor:descriptor];
  MTLRegion region = MTLRegionMake2D(0, 0, srcImage.size.width, srcImage.size.height);
  NSUInteger bytesPerRow = srcImage.size.width * srcImage.pixelBytes;
  [srcTexture replaceRegion:region mipmapLevel:0 withBytes:srcImage.ptr() bytesPerRow:bytesPerRow];
  
  MTLSamplerDescriptor *samplerDesc = [MTLSamplerDescriptor new];
  samplerDesc.rAddressMode = MTLSamplerAddressModeRepeat;
  samplerDesc.sAddressMode = MTLSamplerAddressModeRepeat;
  samplerDesc.minFilter = MTLSamplerMinMagFilterLinear;
  samplerDesc.magFilter = MTLSamplerMinMagFilterLinear;
  samplerDesc.mipFilter = MTLSamplerMipFilterNotMipmapped;
  id<MTLSamplerState> sampler = [_device newSamplerStateWithDescriptor:samplerDesc];
  
  // Setup Pipeline Descriptor
  MTLRenderPipelineDescriptor *pipelineDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
  pipelineDescriptor.colorAttachments[0].pixelFormat = _texture.pixelFormat;
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
  rotationData.push_back(quat::RotZ(0.0f));       // Front
  rotationData.push_back(quat::RotZ(M_PI));       // Back
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
    [renderEncoder setFragmentSamplerState:sampler atIndex:0];
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
