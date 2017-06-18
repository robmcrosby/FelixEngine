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


namespace fx {
  struct MTLGraphicsData {
    id <MTLDevice>       device;
    id <MTLCommandQueue> queue;
    
    CAMetalLayer *layer;
    id<CAMetalDrawable>  drawable;
    id<MTLCommandBuffer> buffer;
    
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
  _data->layer.frame = view.layer.frame;
  [view.layer addSublayer: _data->layer];
  
  return true;
}

FrameBuffer* MetalGraphics::getMainWindowBuffer() {
  return new MetalFrameBuffer(_data->device);
}

ShaderProgram* MetalGraphics::createShaderProgram() {
  return new MetalShaderProgram(_data->device);
}

VertexMesh* MetalGraphics::createVertexMesh() {
  return new MetalVertexMesh(_data->device);
}

void MetalGraphics::nextFrame() {
  _data->drawable = [_data->layer nextDrawable];
  _data->buffer   = [_data->queue commandBuffer];
}

void MetalGraphics::addTask(const GraphicTask &task) {
  MetalFrameBuffer   *frame  = static_cast<MetalFrameBuffer*>(task.frame);
  MetalShaderProgram *shader = static_cast<MetalShaderProgram*>(task.shader);
  MetalVertexMesh    *mesh   = static_cast<MetalVertexMesh*>(task.mesh);
  
  MTLRenderPassDescriptor *descriptor = [[MTLRenderPassDescriptor alloc] init];
  descriptor.colorAttachments[0].texture = _data->drawable.texture;
  descriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
  descriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.0, 104.0/255.0, 5.0/255.0, 1.0);
  
  id <MTLRenderCommandEncoder> encoder = [_data->buffer renderCommandEncoderWithDescriptor:descriptor];
  [encoder setRenderPipelineState:shader->_pipeline];
  mesh->encode(encoder, task.instances);
  [encoder endEncoding];
}

void MetalGraphics::render() {
  [_data->buffer presentDrawable:_data->drawable];
  [_data->buffer commit];
  
  _data->drawable = nil;
  _data->buffer = nil;
}
