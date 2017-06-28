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

#include "MetalFrameBuffer.h"
#include "MetalShaderProgram.h"
#include "MetalVertexMesh.h"
#include "MetalUniformBuffer.h"


namespace fx {
  struct MTLGraphicsData {
    id <MTLDevice>       device;
    id <MTLCommandQueue> queue;
    
    CAMetalLayer *layer;
    id<CAMetalDrawable>  drawable;
    id<MTLCommandBuffer> buffer;
    
    std::map<int, id<MTLDepthStencilState> > depthStencilStates;
    
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
  
  CGSize size = [UIScreen mainScreen].nativeBounds.size;
  CGFloat scale = [UIScreen mainScreen].nativeScale;
  
  // Setup the Metal Layer
  _data->layer = [[CAMetalLayer alloc] init];
  if (_data->layer == nil) {
    cerr << "Error Creating Metal Layer" << endl;
    _data->device = nil;
    _data->queue  = nil;
    return false;
  }
  _data->layer.device = _data->device;
  _data->layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
  _data->layer.framebufferOnly = YES;
  _data->layer.frame = view.bounds;
  _data->layer.contentsScale = scale;
  [view.layer addSublayer: _data->layer];
  
  _frame = new MetalFrameBuffer(_data->device);
  _frame->_colorAttachments.push_back(nil);
  _frame->_size.w = (int)size.width;
  _frame->_size.h = (int)size.height;
  _frame->_scale = (float)scale;
  
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
  _data->drawable = [_data->layer nextDrawable];
  _data->buffer   = [_data->queue commandBuffer];
  _frame->_colorAttachments.at(0) = _data->drawable.texture;
}

void MetalGraphics::addTask(const GraphicTask &task) {
  MetalFrameBuffer   *frame  = static_cast<MetalFrameBuffer*>(task.frame);
  MetalShaderProgram *shader = static_cast<MetalShaderProgram*>(task.shader);
  MetalVertexMesh    *mesh   = static_cast<MetalVertexMesh*>(task.mesh);
  
  // Create Render Encoder
  id <MTLRenderCommandEncoder> encoder = frame->createEncoder(_data->buffer, task);
  
  // Encode the Shader Program and Render Pipeline
  shader->encode(encoder, frame);
  
  // Encode the Uniform Buffers
  for (auto uniform : task.uniforms) {
    MetalUniformBuffer *mtlUniform = (MetalUniformBuffer*)uniform.second;
    mtlUniform->encode(encoder, uniform.first, shader);
  }
  
  // Set the Triangle Culling Mode
  if (task.cullMode == CULL_FRONT)
    [encoder setCullMode:MTLCullModeFront];
  else if (task.cullMode == CULL_BACK)
    [encoder setCullMode:MTLCullModeBack];
  
  // Set the Depth State
  int index = task.depthState.flags;
  if (_data->depthStencilStates.count(index) == 0) {
    MTLDepthStencilDescriptor *depthDesc = [MTLDepthStencilDescriptor new];
    depthDesc.depthWriteEnabled = task.depthState.writingEnabled();
    depthDesc.depthCompareFunction = MTLCompareFunctionAlways;
    
    if (task.depthState.function() == DEPTH_TEST_LESS)
      depthDesc.depthCompareFunction = MTLCompareFunctionLess;
    else if (task.depthState.function() == DEPTH_TEST_GREATER)
      depthDesc.depthCompareFunction = MTLCompareFunctionGreater;
    else if (task.depthState.function() == DEPTH_TEST_EQUAL)
      depthDesc.depthCompareFunction = MTLCompareFunctionEqual;
    else if (task.depthState.function() == DEPTH_TEST_LESS_EQ)
      depthDesc.depthCompareFunction = MTLCompareFunctionLessEqual;
    else if (task.depthState.function() == DEPTH_TEST_GREATER_EQ)
      depthDesc.depthCompareFunction = MTLCompareFunctionGreaterEqual;
    else if (task.depthState.function() == DEPTH_TEST_NEVER)
      depthDesc.depthCompareFunction = MTLCompareFunctionNever;
    
    _data->depthStencilStates[index] = [_data->device newDepthStencilStateWithDescriptor:depthDesc];
  }
  [encoder setDepthStencilState:_data->depthStencilStates.at(index)];
  
  // Encode the Vertex Buffers
  mesh->encode(encoder, shader, task.instances);
  
  [encoder endEncoding];
}

void MetalGraphics::render() {
  _frame->_colorAttachments.at(0) = nil;
  [_data->buffer presentDrawable:_data->drawable];
  [_data->buffer commit];
  
  _data->drawable = nil;
  _data->buffer = nil;
}
