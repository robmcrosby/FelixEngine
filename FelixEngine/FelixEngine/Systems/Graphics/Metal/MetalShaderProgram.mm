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
#include "MetalTextureSampler.h"
#include "GraphicTask.h"
#include <Metal/Metal.h>


using namespace fx;
using namespace std;

unsigned int MetalShaderProgram::shaderCount = 0;

MetalShaderProgram::MetalShaderProgram(id <MTLDevice> device): _device(device), _vertex(nil), _fragment(nil) {
  _shaderId = shaderCount++;
}

MetalShaderProgram::~MetalShaderProgram() {
  
}

bool MetalShaderProgram::loadShaderFunctions(const string &vertex, const string &fragment) {
  _vertex = nil;
  _fragment = nil;
  
  id <MTLLibrary> library = [_device newDefaultLibrary];
  if (library != nil) {
    _vertex = [library newFunctionWithName:[NSString stringWithUTF8String:vertex.c_str()]];
    _fragment = [library newFunctionWithName:[NSString stringWithUTF8String:fragment.c_str()]];
  }
  
  if (_vertex == nil || _fragment == nil) {
    NSBundle *bundle = [NSBundle bundleForClass:[MetalTextureSampler class]];
    NSString *path = [bundle pathForResource:@"default" ofType:@"metallib"];
    library = [_device newLibraryWithFile:path error:nil];
    if (_vertex == nil)
      _vertex = [library newFunctionWithName:[NSString stringWithUTF8String:vertex.c_str()]];
    if (_fragment == nil)
      _fragment = [library newFunctionWithName:[NSString stringWithUTF8String:fragment.c_str()]];
  }
  
  if (_vertex != nil  && _fragment != nil) {
    extractIndexMaps();
    return true;
  }
  return false;
}

void MetalShaderProgram::encode(id <MTLRenderCommandEncoder> encoder, const GraphicTask &task) {
  MetalFrameBuffer *frame = static_cast<MetalFrameBuffer*>(task.frame.get());
  encode(encoder, frame, task.blendState);
  
  for (const auto &uniformMap : task.uniforms)
    encode(encoder, uniformMap);
}

void MetalShaderProgram::encode(id <MTLRenderCommandEncoder> encoder, MetalFrameBuffer *frame, BlendState blendState) {
  int pipelineId = frame->pipelineId(blendState);
  if (!_pipelines.count(pipelineId))
    addPipeline(frame, blendState);
  [encoder setRenderPipelineState:_pipelines.at(pipelineId)];
}

void MetalShaderProgram::encode(id <MTLRenderCommandEncoder> encoder, UniformsPtr uniforms) {
  if (uniforms) {
    for (auto uniform : *uniforms)
      setUniform(encoder, uniform.first, uniform.second);
  }
}

unsigned int MetalShaderProgram::getShaderId() const {
  return _shaderId;
}

void MetalShaderProgram::setUniform(id <MTLRenderCommandEncoder> encoder, const string &name, const Uniform &uniform) {
  if (uniform.usingBuffer()) {
    MetalUniformBuffer *buffer = static_cast<MetalUniformBuffer*>(uniform.buffer().get());
    buffer->encodeCurrent(encoder, name, this);
  }
  else {
    if (_vertexIndexMap.count(name) > 0)
      [encoder setVertexBytes:uniform.ptr() length:uniform.sizeInBytes() atIndex:_vertexIndexMap.at(name)];
    if (_fragmentIndexMap.count(name) > 0)
      [encoder setFragmentBytes:uniform.ptr() length:uniform.sizeInBytes() atIndex:_fragmentIndexMap.at(name)];
  }
}

void MetalShaderProgram::addPipeline(MetalFrameBuffer *frame, const BlendState &blending) {
  MTLRenderPipelineDescriptor *descriptor = frame->createPipelineDescriptor(blending);
  descriptor.vertexFunction = _vertex;
  descriptor.fragmentFunction = _fragment;
  
  int pipelineId = frame->pipelineId(blending);
  _pipelines[pipelineId] = [_device newRenderPipelineStateWithDescriptor:descriptor error:nil];
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
