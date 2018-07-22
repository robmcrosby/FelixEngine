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

#include "MetalFrameBuffer.h"
#include "MetalShaderProgram.h"
#include "MetalVertexMesh.h"
#include "MetalUniformBuffer.h"
#include "MetalTextureBuffer.h"
#include "MetalDepthStencil.h"
#include "MetalTextureSampler.h"
#include "MetalRenderPass.h"

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
    
    // TODO: Put in Better Structure
    std::vector<MetalRenderPass*> renderPasses;
    
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

RenderPassPtr MetalGraphics::createRenderPass() {
  shared_ptr<MetalRenderPass> renderPass = make_shared<MetalRenderPass>(_data->device);
  _data->renderPasses.push_back(renderPass.get());
  return renderPass;
}

void MetalGraphics::nextFrame() {
  dispatch_semaphore_wait(_data->frameBoundarySemaphore, DISPATCH_TIME_FOREVER);
  
  CGFloat width = _data->view.bounds.size.width;
  CGFloat height = _data->view.bounds.size.height;
  if (_data->metalLayer.frame.size.width != width || _data->metalLayer.frame.size.height != height) {
    _data->metalLayer.frame = CGRectMake(0, 0, width, height);
  }
  
  _data->buffer = [_data->queue commandBuffer];
  for (auto &renderPass : _data->renderPasses)
    renderPass->setCommandBuffer(_data->buffer);
}

void MetalGraphics::presentFrame() {
  if (_windowBuffer != nullptr) {
    _windowBuffer->present(_data->buffer);
  
    __weak dispatch_semaphore_t semaphore = _data->frameBoundarySemaphore;
    [_data->buffer addCompletedHandler:^(id<MTLCommandBuffer> commandBuffer) {
      dispatch_semaphore_signal(semaphore);
    }];
    [_data->buffer commit];
  }
  _data->buffer = nil;
}
