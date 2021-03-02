//
//  MetalGraphics.m
//  FelixEngine
//
//  Created by Robert Crosby on 6/14/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include <iostream>

#include "MetalGraphics.h"
#include "MetalContext.h"

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


using namespace std;
using namespace fx;


MetalGraphics::MetalGraphics(): _context(new MetalContext()) {
  Graphics::instance = this;
}

MetalGraphics::~MetalGraphics() {
  delete _context;
}

bool MetalGraphics::initalize(UIView *view) {
  // Get the Metal Device
  _context->device = MTLCreateSystemDefaultDevice();
  if (_context->device == nil) {
    cerr << "Error Creating Metal Device" << endl;
    return false;
  }
  
  // Create a Command Queue
  _context->queue = [_context->device newCommandQueue];
  if (_context->queue == nil) {
    cerr << "Error Creating Metal Queue" << endl;
    _context->device = nil;
    return false;
  }
  
  _context->view = view;
  
  CGFloat width = view.bounds.size.width;
  CGFloat height = view.bounds.size.height;
  
  // Setup the Metal Layer
  _context->metalLayer = [[CAMetalLayer alloc] init];
  if (_context->metalLayer == nil) {
    cerr << "Error Creating Metal Layer" << endl;
    _context->device = nil;
    _context->queue  = nil;
    return false;
  }
  _context->metalLayer.device = _context->device;
  _context->metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
  _context->metalLayer.framebufferOnly = YES;
  _context->metalLayer.frame = CGRectMake(0, 0, width, height);
  _context->metalLayer.contentsScale = [UIScreen mainScreen].nativeScale;
  [view.layer addSublayer: _context->metalLayer];
  
  // setup the Frame Boundry Semaphore
  _context->frameBoundarySemaphore = dispatch_semaphore_create(MAX_INFLIGHT_FRAMES);
  
  return true;
}

FrameBufferPtr MetalGraphics::createFrameBuffer() {
  MetalFrameBufferPtr frame = make_shared<MetalFrameBuffer>(_context->device, _context->queue);
  return frame;
}

ShaderProgramPtr MetalGraphics::createShaderProgram() {
  MetalShaderProgramPtr shader = make_shared<MetalShaderProgram>(_context->device);
  return shader;
}

VertexMeshPtr MetalGraphics::createVertexMesh() {
  MetalVertexMeshPtr mesh = make_shared<MetalVertexMesh>(_context->device);
  return mesh;
}

UniformBufferPtr MetalGraphics::createUniformBuffer() {
  MetalUniformBufferPtr buffer = make_shared<MetalUniformBuffer>(_context->device);
  return buffer;
}

TextureBufferPtr MetalGraphics::createTextureBuffer() {
  MetalTextureBufferPtr texture = make_shared<MetalTextureBuffer>(_context->device, _context->queue);
  return texture;
}

BufferPoolPtr MetalGraphics::createBufferPool() {
  BufferPoolPtr pool = make_shared<BufferPool>();
  return pool;
}

RenderPassPtr MetalGraphics::createRenderPass() {
  MetalRenderPassPtr renderPass = make_shared<MetalRenderPass>(_context->device);
  return renderPass;
}

CommandBufferPtr MetalGraphics::createCommandBuffer() {
  MetalCommandBufferPtr commandBuffer = make_shared<MetalCommandBuffer>([_context->queue commandBuffer]);
  return commandBuffer;
}

void MetalGraphics::nextFrame() {
  dispatch_semaphore_wait(_context->frameBoundarySemaphore, DISPATCH_TIME_FOREVER);
  
  CGFloat width = _context->view.bounds.size.width;
  CGFloat height = _context->view.bounds.size.height;
  if (_context->metalLayer.frame.size.width != width || _context->metalLayer.frame.size.height != height) {
    _context->metalLayer.frame = CGRectMake(0, 0, width, height);
  }
}

void MetalGraphics::presentFrame() {
  if (_context->windowBuffer && _frameCommandBuffer) {
    MetalCommandBufferPtr commandBuffer = static_pointer_cast<MetalCommandBuffer>(_frameCommandBuffer);
    _context->windowBuffer->present(commandBuffer->_buffer);
  
    __weak dispatch_semaphore_t semaphore = _context->frameBoundarySemaphore;
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
  _context->windowBuffer = static_pointer_cast<MetalFrameBuffer>(renderPass->getFrame());
  _context->windowBuffer->setMetalLayer(_context->metalLayer);
  _context->renderPasses.push_back(renderPass);
}
