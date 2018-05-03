//
//  VulkanGraphics.mm
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "VulkanGraphics.h"

#include "VulkanFrameBuffer.h"
#include "VulkanShaderProgram.h"
#include "VulkanVertexMesh.h"
#include "VulkanUniformBuffer.h"
#include "VulkanTextureBuffer.h"


using namespace std;
using namespace fx;

VulkanGraphics::VulkanGraphics() {
  
}

VulkanGraphics::~VulkanGraphics() {
  
}

bool VulkanGraphics::initalize(UIView *view) {
  bool success = true;
  
  return success;
}

FrameBufferPtr VulkanGraphics::createFrameBuffer() {
  shared_ptr<VulkanFrameBuffer> buffer = make_shared<VulkanFrameBuffer>();
  return buffer;
}

ShaderProgramPtr VulkanGraphics::createShaderProgram() {
  shared_ptr<VulkanShaderProgram> shader = make_shared<VulkanShaderProgram>();
  return shader;
}

VertexMeshPtr VulkanGraphics::createVertexMesh() {
  shared_ptr<VulkanVertexMesh> mesh = make_shared<VulkanVertexMesh>();
  return mesh;
}

UniformBufferPtr VulkanGraphics::createUniformBuffer() {
  shared_ptr<VulkanUniformBuffer> buffer = make_shared<VulkanUniformBuffer>();
  return buffer;
}

TextureBufferPtr VulkanGraphics::createTextureBuffer() {
  shared_ptr<VulkanTextureBuffer> texture = make_shared<VulkanTextureBuffer>();
  return texture;
}

void VulkanGraphics::nextFrame() {
  
}

void VulkanGraphics::addTask(const GraphicTask &task) {
  
}

void VulkanGraphics::presentFrame() {
  
}
