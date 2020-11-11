//
//  MetalRenderPass.m
//  FelixEngine
//
//  Created by Robert Crosby on 7/7/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "MetalRenderPass.h"
#include <Metal/Metal.h>

#include "GraphicResources.h"

#include "MetalFrameBuffer.h"
#include "MetalShaderProgram.h"
#include "MetalVertexMesh.h"
#include "MetalUniformBuffer.h"
#include "MetalTextureBuffer.h"
#include "MetalDepthStencil.h"
#include "MetalTextureSampler.h"


using namespace fx;
using namespace std;

MetalRenderPass::MetalRenderPass(id <MTLDevice> device): _device(device) {
  _depthStencilStates = [[MetalDepthStencil alloc] initWithDevice:_device];
  _samplerStates = [[MetalTextureSampler alloc] initWithDevice:_device];
}

MetalRenderPass::~MetalRenderPass() {

}

void MetalRenderPass::render() {
  MetalFrameBuffer *mtlFrame  = static_cast<MetalFrameBuffer*>(frame.get());

  // Create Render Encoder
  id <MTLRenderCommandEncoder> encoder = mtlFrame->createEncoder(_buffer, depthStencilAction, colorActions);
  
  sortRenderItems();
  for (auto &item : renderItems) {
    MetalShaderProgram *shader = static_cast<MetalShaderProgram*>(item.shader.get());
    MetalVertexMesh    *mesh   = static_cast<MetalVertexMesh*>(item.mesh.get());

    // Encode the Shader Program, Render Pipeline
    shader->encode(encoder, mtlFrame, item.blendState);

    // Encode the Uniform Buffers to the Shader
    for (const auto &uniformMap : uniforms)
      shader->encode(encoder, uniformMap);
    for (const auto &uniformMap : item.uniforms)
      shader->encode(encoder, uniformMap);
    
    // Set the Depth/Stencil State
    [encoder setDepthStencilState:[_depthStencilStates depthStencilStateForFlags:item.depthState.flags]];
    
    // Set Blending Color
    if (item.blendState.enabled()) {
      [encoder setBlendColorRed:item.blendState.color.r
                          green:item.blendState.color.g
                           blue:item.blendState.color.b
                          alpha:item.blendState.color.a];
    }
    
    // Set Culling Mode
    if (item.cullMode == CULL_BACK)
      [encoder setCullMode:MTLCullModeBack];
    else if (item.cullMode == CULL_FRONT)
      [encoder setCullMode:MTLCullModeFront];
    
    // Set the Textures
    if (item.textures) {
      for (auto &texture : *item.textures) {
        int index = (int)shader->getTextureIndex(texture.first);
        if (index >= 0) {
          id <MTLSamplerState> sampler = [_samplerStates samplerStateForFlags:texture.second.sampler.flags];
          MetalTextureBuffer *mtlTextureBuffer = static_cast<MetalTextureBuffer*>(texture.second.buffer.get());
          mtlTextureBuffer->encode(encoder, sampler, index);
        }
      }
    }
    
    // Encode the Vertex Buffers
    mesh->encode(encoder, shader, item.instances);
  }
  // End Encoding
  [encoder endEncoding];
}

void MetalRenderPass::setCommandBuffer(id <MTLCommandBuffer> buffer) {
  _buffer = buffer;
}
