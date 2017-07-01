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
#include "MetalDepthStencil.h"

#define MAX_INFLIGHT_FRAMES 3

namespace fx {
  struct MTLGraphicsData {
    id <MTLDevice>        device;
    id <MTLCommandQueue>  queue;
    id <MTLCommandBuffer> buffer;
    
    MetalDepthStencil *depthStencilStates;
    
    dispatch_semaphore_t frameBoundarySemaphore;
    
    ~MTLGraphicsData() {}
  };
}


using namespace std;
using namespace fx;


MetalGraphics::MetalGraphics(): _data(new MTLGraphicsData()) {
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
  
  // Setup the Metal Layer
  CAMetalLayer *layer = [[CAMetalLayer alloc] init];
  if (layer == nil) {
    cerr << "Error Creating Metal Layer" << endl;
    _data->device = nil;
    _data->queue  = nil;
    return false;
  }
  layer.device = _data->device;
  layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
  layer.framebufferOnly = YES;
  layer.frame = view.bounds;
  layer.contentsScale = [UIScreen mainScreen].nativeScale;
  [view.layer addSublayer: layer];
  
  // Set the Metal Layer to a Frame Buffer
  _frame = new MetalFrameBuffer(_data->device);
  _frame->setMetalLayer(layer);
  
  // Create an instance of MetalDepthStencil
  _data->depthStencilStates = [[MetalDepthStencil alloc] initWithDevice:_data->device];
  
  // setup the Frame Boundry Semaphore
  _data->frameBoundarySemaphore = dispatch_semaphore_create(MAX_INFLIGHT_FRAMES);
  
  return true;
}

FrameBuffer* MetalGraphics::getMainWindowBuffer() {
  return _frame;
}

ShaderProgram* MetalGraphics::createShaderProgram() {
  return new MetalShaderProgram(_data->device);
}

VertexMesh* MetalGraphics::createVertexMesh() {
  return new MetalVertexMesh(_data->device);
}

UniformBuffer* MetalGraphics::createUniformBuffer() {
  return new MetalUniformBuffer(_data->device);
}

void MetalGraphics::nextFrame() {
  dispatch_semaphore_wait(_data->frameBoundarySemaphore, DISPATCH_TIME_FOREVER);
  _data->buffer = [_data->queue commandBuffer];
}

void MetalGraphics::addTask(const GraphicTask &task) {
  MetalFrameBuffer   *frame  = static_cast<MetalFrameBuffer*>(task.frame);
  MetalShaderProgram *shader = static_cast<MetalShaderProgram*>(task.shader);
  MetalVertexMesh    *mesh   = static_cast<MetalVertexMesh*>(task.mesh);
  
  // Create Render Encoder
  id <MTLRenderCommandEncoder> encoder = frame->createEncoder(_data->buffer, task);
  
  // Encode the Shader Program, Render Pipeline, and Uniform Buffers
  shader->encode(encoder, task);
  
  // Set the Depth/Stencil State
  int flags = task.depthStencilState.flags;
  [encoder setDepthStencilState:[_data->depthStencilStates depthStencilStateForFlags:flags]];
  
  // Encode the Vertex Buffers and End Encoding
  mesh->encode(encoder, shader, task.instances);
  [encoder endEncoding];
}

void MetalGraphics::presentFrame() {
  _frame->present(_data->buffer);
  
  __weak dispatch_semaphore_t semaphore = _data->frameBoundarySemaphore;
  [_data->buffer addCompletedHandler:^(id<MTLCommandBuffer> commandBuffer) {
    dispatch_semaphore_signal(semaphore);
  }];
  
  [_data->buffer commit];
  _data->buffer = nil;
}
