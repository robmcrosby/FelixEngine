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
#include "MetalTextureBuffer.h"
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

void MetalShaderProgram::encode(id <MTLRenderCommandEncoder> encoder, MetalFrameBuffer *frame, BlendState blendState) {
  int pipelineId = frame->pipelineId(blendState);
  if (!_pipelines.count(pipelineId))
    addPipeline(frame, blendState);
  [encoder setRenderPipelineState:_pipelines.at(pipelineId)];
}

void MetalShaderProgram::encode(id <MTLRenderCommandEncoder> encoder, UniformsPtr uniforms) {
  if (uniforms) {
    for (auto &uniform : *uniforms)
      setUniform(encoder, uniform.first, uniform.second);
  }
}

void MetalShaderProgram::encode(id <MTLRenderCommandEncoder> encoder, TexturesPtr textures, MetalTextureSampler *samplers) {
  if (textures) {
    for (auto &texture : *textures)
      setTexture(encoder, texture.first, texture.second, samplers);
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

void MetalShaderProgram::setTexture(id <MTLRenderCommandEncoder> encoder, const string &name, const Texture &texture, MetalTextureSampler *samplers) {
  if (_textureIndexMap.count(name) > 0 && texture.buffer) {
    id <MTLSamplerState> sampler = [samplers samplerStateForFlags:texture.sampler.flags];
    MetalTextureBuffer *mtlTextureBuffer = static_cast<MetalTextureBuffer*>(texture.buffer.get());
    mtlTextureBuffer->encode(encoder, sampler, _textureIndexMap.at(name));
  }
}

void MetalShaderProgram::setDescriptorAttributes(MTLRenderPipelineDescriptor *descriptor) {
  for (auto itr = _attributeTypeMap.begin(); itr != _attributeTypeMap.end(); ++itr) {
    long index = _vertexIndexMap[itr->first];
    
    MTLDataType dataType = (MTLDataType)itr->second;
    MTLVertexFormat format = MTLVertexFormatFloat;
    long stride = sizeof(float);
    if (dataType == MTLDataTypeFloat2) {
      stride = sizeof(float2);
      format = MTLVertexFormatFloat2;
    }
    else if (dataType == MTLDataTypeFloat3) {
      stride = sizeof(float3);
      format = MTLVertexFormatFloat3;
    }
    else if (dataType == MTLDataTypeFloat4) {
      stride = sizeof(float4);
      format = MTLVertexFormatFloat4;
    }
    
    descriptor.vertexDescriptor.attributes[index].bufferIndex = index;
    descriptor.vertexDescriptor.attributes[index].offset = 0;
    descriptor.vertexDescriptor.attributes[index].format = format;
    descriptor.vertexDescriptor.layouts[index].stride = stride;
  }
}

void MetalShaderProgram::addPipeline(MetalFrameBuffer *frame, const BlendState &blending) {
  MTLRenderPipelineDescriptor *descriptor = frame->createPipelineDescriptor(blending);
  descriptor.vertexFunction = _vertex;
  descriptor.fragmentFunction = _fragment;
  setDescriptorAttributes(descriptor);
  
  int pipelineId = frame->pipelineId(blending);
  _pipelines[pipelineId] = [_device newRenderPipelineStateWithDescriptor:descriptor error:nil];
}

void MetalShaderProgram::extractIndexMaps() {
  // Get Vertex Attributes from Vertex Shader
  for (MTLVertexAttribute *att in _vertex.vertexAttributes) {
    _attributeTypeMap[[att.name UTF8String]] = att.attributeType;
    _vertexIndexMap[[att.name UTF8String]] = att.attributeIndex;
  }
  
  // Create a basic Pipeline Descriptor
  MTLRenderPipelineDescriptor *descriptor = [[MTLRenderPipelineDescriptor alloc] init];
  descriptor.vertexFunction = _vertex;
  descriptor.fragmentFunction = _fragment;
  descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatRGBA8Unorm;
  setDescriptorAttributes(descriptor);
  
  // Create the Pipeline with Reflection
  MTLRenderPipelineReflection *reflection;
  [_device newRenderPipelineStateWithDescriptor:descriptor
                                        options:MTLPipelineOptionArgumentInfo
                                     reflection:&reflection
                                          error:nil];
  
  // Extract the Reflection Information
  for (MTLArgument *arg in reflection.vertexArguments)
    _vertexIndexMap[[arg.name UTF8String]] = arg.index;
  
  for (MTLArgument *arg in reflection.fragmentArguments) {
    if (arg.type == MTLArgumentTypeTexture)
      _textureIndexMap[[arg.name UTF8String]] = arg.index;
    else if (arg.type == MTLArgumentTypeSampler)
      _samplerIndexMap[[arg.name UTF8String]] = arg.index;
    else
      _fragmentIndexMap[[arg.name UTF8String]] = arg.index;
  }
}
