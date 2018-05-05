//
//  VulkanGraphics.mm
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "VulkanGraphics.h"

#import "MetalView.h"

#include "Vulkan.h"
#include <vulkan/vulkan_ios.h>

#include "VulkanFrameBuffer.h"
#include "VulkanShaderProgram.h"
#include "VulkanVertexMesh.h"
#include "VulkanUniformBuffer.h"
#include "VulkanTextureBuffer.h"


using namespace std;
using namespace fx;

VulkanGraphics::VulkanGraphics() {
  _layers.push_back("MoltenVK");
  _extensions.push_back("VK_MVK_moltenvk");
}

VulkanGraphics::~VulkanGraphics() {
  
}

bool VulkanGraphics::initalize(UIView *view) {
  bool success = true;
  
  CGFloat viewWidth = view.frame.size.width;
  CGFloat viewHeight = view.frame.size.height;
  CGRect frame = CGRectMake(0, 0, viewWidth, viewHeight);
  MetalView *mtlView = [[MetalView alloc] initWithFrame:frame];
  [view addSubview:mtlView];
  
  assert(createInstance());
  
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

bool VulkanGraphics::createInstance() {
  // Provide Application Information
  VkApplicationInfo appInfo;
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Felix Application";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "Felix Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;
  
  return Vulkan::createInstance(appInfo, _layers, _extensions);
}
