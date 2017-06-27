//
//  MetalUniformBuffer.m
//  FelixEngine
//
//  Created by Robert Crosby on 6/26/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "MetalUniformBuffer.h"

using namespace fx;
using namespace std;


MetalUniformBuffer::MetalUniformBuffer(id <MTLDevice> device): _device(device) {
  
}

MetalUniformBuffer::~MetalUniformBuffer() {
  
}

bool MetalUniformBuffer::setNumberOfBuffers(int count) {
  return false;
}

bool MetalUniformBuffer::update(const void *data, size_t size) {
  return false;
}
