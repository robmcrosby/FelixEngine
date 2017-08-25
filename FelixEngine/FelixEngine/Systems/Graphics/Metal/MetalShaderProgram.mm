//
//  MetalShaderProgram.mm
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "MetalShaderProgram.h"
#include "MetalFrameBuffer.h"
#include "MetalUniformBuffer.h"
#include "GraphicTask.h"
#include "UniformMap.h"
#include <Metal/Metal.h>

using namespace fx;
using namespace std;


MetalShaderProgram::MetalShaderProgram(id <MTLDevice> device): _device(device), _vertex(nil), _fragment(nil) {
  
}

MetalShaderProgram::~MetalShaderProgram() {
  
}

bool MetalShaderProgram::loadShaderFunctions(const string &vertex, const string &fragment) {
  id <MTLLibrary> library = [_device newDefaultLibrary];
  _vertex = [library newFunctionWithName:[NSString stringWithUTF8String:vertex.c_str()]];
  _fragment = [library newFunctionWithName:[NSString stringWithUTF8String:fragment.c_str()]];
  
  extractIndexMaps();
  return _vertex != nil  && _fragment != nil;
}

void MetalShaderProgram::encode(id <MTLRenderCommandEncoder> encoder, MetalFrameBuffer *frame) {
  if (!_pipelines.count(frame)) {
    MTLRenderPipelineDescriptor *descriptor = [[MTLRenderPipelineDescriptor alloc] init];
    descriptor.vertexFunction = _vertex;
    descriptor.fragmentFunction = _fragment;
    
    int index = 0;
    for (id <MTLTexture> attachment : frame->_colorAttachments) {
      descriptor.colorAttachments[index].pixelFormat = attachment.pixelFormat;
    }
    
    if (frame->_depthAttachment != nil) {
      descriptor.depthAttachmentPixelFormat = frame->_depthAttachment.pixelFormat;
    }
    if (frame->_stencilAttachment != nil) {
      descriptor.stencilAttachmentPixelFormat = frame->_stencilAttachment.pixelFormat;
    }
    
    _pipelines[frame] = [_device newRenderPipelineStateWithDescriptor:descriptor error:nil];
  }
  
  [encoder setRenderPipelineState:_pipelines.at(frame)];
}

void MetalShaderProgram::encode(id <MTLRenderCommandEncoder> encoder, const GraphicTask &task) {
  MetalFrameBuffer *frame = static_cast<MetalFrameBuffer*>(task.frame);
  if (!_pipelines.count(frame))
    addPipelineForFrame(frame);
  [encoder setRenderPipelineState:_pipelines.at(frame)];
  
  if (task.uniforms != nullptr) {
    for (auto itr : *task.uniforms)
      setUniform(encoder, itr.first, itr.second);
  }
}

void MetalShaderProgram::setUniform(id <MTLRenderCommandEncoder> encoder, const string &name, const Uniform &uniform) {
  if (uniform.usingBuffer()) {
    MetalUniformBuffer *buffer = static_cast<MetalUniformBuffer*>(uniform.buffer());
    buffer->encodeCurrent(encoder, name, this);
  }
  else {
    if (_vertexIndexMap.count(name) > 0)
      [encoder setVertexBytes:uniform.ptr() length:uniform.sizeInBytes() atIndex:_vertexIndexMap.at(name)];
    if (_fragmentIndexMap.count(name) > 0)
      [encoder setFragmentBytes:uniform.ptr() length:uniform.sizeInBytes() atIndex:_fragmentIndexMap.at(name)];
  }
}

void MetalShaderProgram::addPipelineForFrame(MetalFrameBuffer *frame) {
  MTLRenderPipelineDescriptor *descriptor = [[MTLRenderPipelineDescriptor alloc] init];
  descriptor.vertexFunction = _vertex;
  descriptor.fragmentFunction = _fragment;
  
  int index = 0;
  for (id <MTLTexture> attachment : frame->_colorAttachments) {
    descriptor.colorAttachments[index].pixelFormat = attachment.pixelFormat;
  }
  
  if (frame->_depthAttachment != nil) {
    descriptor.depthAttachmentPixelFormat = frame->_depthAttachment.pixelFormat;
  }
  if (frame->_stencilAttachment != nil) {
    descriptor.stencilAttachmentPixelFormat = frame->_stencilAttachment.pixelFormat;
  }
  
  _pipelines[frame] = [_device newRenderPipelineStateWithDescriptor:descriptor error:nil];
}

void MetalShaderProgram::extractIndexMaps() {
  // Create a basic Pipeline Descriptor
  MTLRenderPipelineDescriptor *descriptor = [[MTLRenderPipelineDescriptor alloc] init];
  descriptor.vertexFunction = _vertex;
  descriptor.fragmentFunction = _fragment;
  descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatRGBA8Unorm;
  
  // Create the Pipeline with Reflection
  MTLRenderPipelineReflection *reflection;
  [_device newRenderPipelineStateWithDescriptor:descriptor
                                        options:MTLPipelineOptionArgumentInfo
                                     reflection:&reflection
                                          error:nil];
  
  // Extract the Reflection Information
  for (MTLArgument *arg in reflection.vertexArguments)
    _vertexIndexMap[[arg.name UTF8String]] = arg.index;
  for (MTLArgument *arg in reflection.fragmentArguments)
    _fragmentIndexMap[[arg.name UTF8String]] = arg.index;
}
