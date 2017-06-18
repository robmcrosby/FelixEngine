//
//  MetalShaderProgram.mm
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "MetalShaderProgram.h"
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
  
  MTLRenderPipelineDescriptor *descriptor = [[MTLRenderPipelineDescriptor alloc] init];
  descriptor.vertexFunction = _vertex;
  descriptor.fragmentFunction = _fragment;
  descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
  
  _pipeline = [_device newRenderPipelineStateWithDescriptor:descriptor error:nil];
  
  return _vertex != nil  && _fragment != nil;
}
