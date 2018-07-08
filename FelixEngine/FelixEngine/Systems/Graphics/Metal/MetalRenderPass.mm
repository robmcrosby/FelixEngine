//
//  MetalRenderPass.m
//  FelixEngine
//
//  Created by Robert Crosby on 7/7/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "MetalRenderPass.h"
#include <Metal/Metal.h>

using namespace fx;
using namespace std;


MetalRenderPass::MetalRenderPass(id <MTLDevice> device): _device(device) {

}

MetalRenderPass::~MetalRenderPass() {

}

void MetalRenderPass::render() {
  
}
