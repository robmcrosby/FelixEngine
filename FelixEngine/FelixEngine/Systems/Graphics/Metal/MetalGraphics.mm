//
//  MetalGraphics.m
//  FelixEngine
//
//  Created by Robert Crosby on 6/14/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include <iostream>

#include "MetalGraphics.h"
#include <Metal/Metal.h>
#include <UIKit/UIKit.h>
#include <QuartzCore/CAMetalLayer.h>

#include "GraphicResources.h"
#include "GraphicTask.h"

#include "MetalFrameBuffer.h"
#include "MetalShaderProgram.h"
#include "MetalVertexMesh.h"
#include "MetalUniformBuffer.h"
#include "MetalTextureBuffer.h"
#include "MetalDepthStencil.h"
#include "MetalTextureSampler.h"

#define MAX_INFLIGHT_FRAMES 3

namespace fx {
  struct MTLGraphicsData {
    id <MTLDevice>        device;
    id <MTLCommandQueue>  queue;
    id <MTLCommandBuffer> buffer;
    
    UIView *view;
    CAMetalLayer *metalLayer;
    MetalDepthStencil *depthStencilStates;
    MetalTextureSampler *samplerStates;
    
    dispatch_semaphore_t frameBoundarySemaphore;
    
    ~MTLGraphicsData() {}
  };
}


using namespace std;
using namespace fx;


MetalGraphics::MetalGraphics(): _data(new MTLGraphicsData()), _windowBuffer(nullptr) {
  Graphics::instance = this;
}

MetalGraphics::~MetalGraphics() {
  delete _data;
}

bool MetalGraphics::initalize(UIView *view) {
  // Get the Metal Device
  _data->device = MTLCreateSystemDefaultDevice();
  if (_data->device == nil) {
    cerr << "Error Creating Metal Device" << endl;
    return false;
  }
  
  // Create a Command Queue
  _data->queue = [_data->device newCommandQueue];
  if (_data->queue == nil) {
    cerr << "Error Creating Metal Queue" << endl;
    _data->device = nil;
    return false;
  }
  
  _data->view = view;
  
  CGFloat width = view.bounds.size.width;
  CGFloat height = view.bounds.size.height;
  
  // Setup the Metal Layer
  _data->metalLayer = [[CAMetalLayer alloc] init];
  if (_data->metalLayer == nil) {
    cerr << "Error Creating Metal Layer" << endl;
    _data->device = nil;
    _data->queue  = nil;
    return false;
  }
  _data->metalLayer.device = _data->device;
  _data->metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
  _data->metalLayer.framebufferOnly = YES;
  _data->metalLayer.frame = CGRectMake(0, 0, width, height);
  _data->metalLayer.contentsScale = [UIScreen mainScreen].nativeScale;
  [view.layer addSublayer: _data->metalLayer];
  
  // Create an instance of MetalDepthStencil
  _data->depthStencilStates = [[MetalDepthStencil alloc] initWithDevice:_data->device];
  
  // Create an instance of MetalTextureSampler
  _data->samplerStates = [[MetalTextureSampler alloc] initWithDevice:_data->device];
  
  // setup the Frame Boundry Semaphore
  _data->frameBoundarySemaphore = dispatch_semaphore_create(MAX_INFLIGHT_FRAMES);
  
  return true;
}

bool MetalGraphics::setWindowBuffer(MetalFrameBuffer *buffer, int index) {
  _windowBuffer = buffer;
  _windowBuffer->setMetalLayer(_data->metalLayer);
  return false;
}

FrameBufferPtr MetalGraphics::createFrameBuffer() {
  shared_ptr<MetalFrameBuffer> frame = make_shared<MetalFrameBuffer>(_data->device);
  return frame;
}

ShaderProgramPtr MetalGraphics::createShaderProgram() {
  shared_ptr<MetalShaderProgram> shader = make_shared<MetalShaderProgram>(_data->device);
  return shader;
}

VertexMeshPtr MetalGraphics::createVertexMesh() {
  shared_ptr<MetalVertexMesh> mesh = make_shared<MetalVertexMesh>(_data->device);
  return mesh;
}

UniformBufferPtr MetalGraphics::createUniformBuffer() {
  shared_ptr<MetalUniformBuffer> buffer = make_shared<MetalUniformBuffer>(_data->device);
  return buffer;
}

TextureBufferPtr MetalGraphics::createTextureBuffer() {
  shared_ptr<MetalTextureBuffer> texture = make_shared<MetalTextureBuffer>(_data->device);
  return texture;
}

BufferPoolPtr MetalGraphics::createBufferPool() {
  BufferPoolPtr pool = make_shared<BufferPool>();
  return pool;
}

void MetalGraphics::nextFrame() {
  dispatch_semaphore_wait(_data->frameBoundarySemaphore, DISPATCH_TIME_FOREVER);
  
  CGFloat width = _data->view.bounds.size.width;
  CGFloat height = _data->view.bounds.size.height;
  if (_data->metalLayer.frame.size.width != width || _data->metalLayer.frame.size.height != height) {
    _data->metalLayer.frame = CGRectMake(0, 0, width, height);
  }
  
  _data->buffer = [_data->queue commandBuffer];
}

void MetalGraphics::addTask(const GraphicTask &task) {
  MetalFrameBuffer   *frame  = static_cast<MetalFrameBuffer*>(task.frame.get());
  MetalShaderProgram *shader = static_cast<MetalShaderProgram*>(task.shader.get());
  MetalVertexMesh    *mesh   = static_cast<MetalVertexMesh*>(task.mesh.get());
  
  // Create Render Encoder
  id <MTLRenderCommandEncoder> encoder = frame->createEncoder(_data->buffer, task);
  
  // Encode the Shader Program, Render Pipeline, and Uniform Buffers
  shader->encode(encoder, task);
  
  // Set the Depth/Stencil State
  int flags = task.depthState.flags;
  [encoder setDepthStencilState:[_data->depthStencilStates depthStencilStateForFlags:flags]];
  
  // Set Blending Color
  if (task.blendState.enabled()) {
    [encoder setBlendColorRed:task.blendState.color.r
                        green:task.blendState.color.g
                         blue:task.blendState.color.b
                        alpha:task.blendState.color.a];
  }
  
  // Set Culling Mode
  if (task.cullMode == CULL_BACK)
    [encoder setCullMode:MTLCullModeBack];
  else if (task.cullMode == CULL_FRONT)
    [encoder setCullMode:MTLCullModeFront];
  
  // Set the Textures
  if (task.textures != nullptr) {
    int index = 0;
    for (auto texture : *task.textures) {
      id <MTLSamplerState> sampler = [_data->samplerStates samplerStateForFlags:texture.sampler.flags];
      MetalTextureBuffer *mtlTextureBuffer = static_cast<MetalTextureBuffer*>(texture.buffer.get());
      mtlTextureBuffer->encode(encoder, sampler, index);
      ++index;
    }
  }
  
  // Encode the Vertex Buffers and End Encoding
  mesh->encode(encoder, shader, task.instances);
  [encoder endEncoding];
}

void MetalGraphics::presentFrame() {
  _windowBuffer->present(_data->buffer);
  
  __weak dispatch_semaphore_t semaphore = _data->frameBoundarySemaphore;
  [_data->buffer addCompletedHandler:^(id<MTLCommandBuffer> commandBuffer) {
    dispatch_semaphore_signal(semaphore);
  }];
  
  [_data->buffer commit];
  _data->buffer = nil;
}
