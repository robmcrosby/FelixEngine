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
#include "MetalCommandBuffer.h"

#define MAX_INFLIGHT_FRAMES 3

namespace fx {
  struct MTLGraphicsData {
    id <MTLDevice>        device;
    id <MTLCommandQueue>  queue;
    
    UIView *view;
    CAMetalLayer *metalLayer;
    
    dispatch_semaphore_t frameBoundarySemaphore;
    
    std::vector<MetalRenderPassPtr> renderPasses;
    MetalFrameBufferPtr windowBuffer;
    
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
  
  // setup the Frame Boundry Semaphore
  _data->frameBoundarySemaphore = dispatch_semaphore_create(MAX_INFLIGHT_FRAMES);
  
  return true;
}

FrameBufferPtr MetalGraphics::createFrameBuffer() {
  MetalFrameBufferPtr frame = make_shared<MetalFrameBuffer>(_data->device, _data->queue);
  return frame;
}

ShaderProgramPtr MetalGraphics::createShaderProgram() {
  MetalShaderProgramPtr shader = make_shared<MetalShaderProgram>(_data->device);
  return shader;
}

VertexMeshPtr MetalGraphics::createVertexMesh() {
  MetalVertexMeshPtr mesh = make_shared<MetalVertexMesh>(_data->device);
  return mesh;
}

UniformBufferPtr MetalGraphics::createUniformBuffer() {
  MetalUniformBufferPtr buffer = make_shared<MetalUniformBuffer>(_data->device);
  return buffer;
}

TextureBufferPtr MetalGraphics::createTextureBuffer() {
  MetalTextureBufferPtr texture = make_shared<MetalTextureBuffer>(_data->device, _data->queue);
  return texture;
}

BufferPoolPtr MetalGraphics::createBufferPool() {
  BufferPoolPtr pool = make_shared<BufferPool>();
  return pool;
}

RenderPassPtr MetalGraphics::createRenderPass() {
  MetalRenderPassPtr renderPass = make_shared<MetalRenderPass>(_data->device);
  return renderPass;
}

CommandBufferPtr MetalGraphics::createCommandBuffer() {
  MetalCommandBufferPtr commandBuffer = make_shared<MetalCommandBuffer>([_data->queue commandBuffer]);
  return commandBuffer;
}

void MetalGraphics::nextFrame() {
  dispatch_semaphore_wait(_data->frameBoundarySemaphore, DISPATCH_TIME_FOREVER);
  
  CGFloat width = _data->view.bounds.size.width;
  CGFloat height = _data->view.bounds.size.height;
  if (_data->metalLayer.frame.size.width != width || _data->metalLayer.frame.size.height != height) {
    _data->metalLayer.frame = CGRectMake(0, 0, width, height);
  }
}

void MetalGraphics::presentFrame() {
  if (_data->windowBuffer && _frameCommandBuffer) {
    MetalCommandBufferPtr commandBuffer = static_pointer_cast<MetalCommandBuffer>(_frameCommandBuffer);
    _data->windowBuffer->present(commandBuffer->_buffer);
  
    __weak dispatch_semaphore_t semaphore = _data->frameBoundarySemaphore;
    [commandBuffer->_buffer addCompletedHandler:^(id<MTLCommandBuffer> commandBuffer) {
      dispatch_semaphore_signal(semaphore);
    }];
    commandBuffer->commit();
  }
  _frameCommandBuffer = nullptr;
}

CommandBufferPtr MetalGraphics::getFrameCommandBuffer() {
  if (!_frameCommandBuffer)
    _frameCommandBuffer = createCommandBuffer();
  return _frameCommandBuffer;
}

void MetalGraphics::setRenderPassToWindow(RenderPassPtr pass, int window) {
  MetalRenderPassPtr renderPass = static_pointer_cast<MetalRenderPass>(pass);
  _data->windowBuffer = static_pointer_cast<MetalFrameBuffer>(renderPass->getFrame());
  _data->windowBuffer->setMetalLayer(_data->metalLayer);
  _data->renderPasses.push_back(renderPass);
}
