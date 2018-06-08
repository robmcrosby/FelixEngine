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

#include "GraphicResources.h"
#include "GraphicTask.h"

#include "VulkanFrameBuffer.h"
#include "VulkanShaderProgram.h"
#include "VulkanVertexMesh.h"
#include "VulkanUniformBuffer.h"
#include "VulkanTextureBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanBufferPool.h"


using namespace std;
using namespace fx;

VulkanGraphics::VulkanGraphics() {
  Graphics::instance = this;
  
  _layers.push_back("MoltenVK");
  _extensions.push_back("VK_MVK_moltenvk");
}

VulkanGraphics::~VulkanGraphics() {
  Vulkan::waitIdle();
  clearCommandPools();
  Vulkan::cleaup();
}

bool VulkanGraphics::initalize(UIView *view) {
  bool success = true;
  
  // Create an Embeded Metal View
  CGFloat viewWidth = view.frame.size.width;
  CGFloat viewHeight = view.frame.size.height;
  CGRect frame = CGRectMake(0, 0, viewWidth, viewHeight);
  MetalView *mtlView = [[MetalView alloc] initWithFrame:frame];
  [view addSubview:mtlView];
  
  float width = viewWidth;
  float height = viewHeight;
  
  assert(createInstance());
  assert(Vulkan::setDefaultPhysicalDevice());
  assert(Vulkan::setDefaultQueueFamily());
  assert(createSwapChain(mtlView));
  assert(Vulkan::createLogicalDevice());
  assert(Vulkan::initDeviceQueue(width, height));
  
  assert(createRenderPass());
  assert(createCommandPool());
  assert(Vulkan::createSemaphores());
  
  createCommandPools();
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

BufferPoolPtr VulkanGraphics::createBufferPool() {
  shared_ptr<VulkanBufferPool> pool = make_shared<VulkanBufferPool>();
  return pool;
}

void VulkanGraphics::nextFrame() {
  vkAcquireNextImageKHR(Vulkan::device, Vulkan::swapChain, std::numeric_limits<uint64_t>::max(), Vulkan::imageAvailableSemaphore, VK_NULL_HANDLE, &Vulkan::currentSwapBuffer);
  _renderPasses.clear();
}

void VulkanGraphics::addTask(const GraphicTask &task) {
  // TODO: Fix for multiple Tasks in Same Render Pass
  if (_renderPasses.empty())
    _renderPasses.push_back({task});
  else
    _renderPasses.back().push_back(task);
}

void VulkanGraphics::presentFrame() {
  _commandPools[Vulkan::currentSwapBuffer]->submitRenderPasses(_renderPasses);
  
  VkSemaphore signalSemaphores[] = {Vulkan::renderFinishedSemaphore};
  VkPresentInfoKHR presentInfo = {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;
  
  VkSwapchainKHR swapChains[] = {Vulkan::swapChain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  
  presentInfo.pImageIndices = &Vulkan::currentSwapBuffer;
  
  vkQueuePresentKHR(Vulkan::presentQueue, &presentInfo);
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

bool VulkanGraphics::createSwapChain(UIView *view) {
  VkIOSSurfaceCreateInfoMVK createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK;
  createInfo.pNext = NULL;
  createInfo.flags = 0;
  createInfo.pView = (__bridge const void*)view;
  
  if (vkCreateIOSSurfaceMVK(Vulkan::instance, &createInfo, nullptr, &Vulkan::surface) != VK_SUCCESS) {
    cerr << "Error creating Vulkan Surface" << endl;
    return false;
  }
  
  VkBool32 supportsPresent;
  vkGetPhysicalDeviceSurfaceSupportKHR(Vulkan::physicalDevice, Vulkan::queueFamilyIndex, Vulkan::surface, &supportsPresent);
  if (!supportsPresent) {
    cerr << "Present Surface not supported with selected Queue Family" << endl;
    return false;
  }
  
  vector<VkSurfaceFormatKHR> formats = Vulkan::getPhysicalDeviceSurfaceFormats(Vulkan::physicalDevice, Vulkan::surface);
  assert(formats.size() > 0);
  if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
    Vulkan::surfaceFormat = VK_FORMAT_B8G8R8A8_UNORM;
  else
    Vulkan::surfaceFormat = formats[0].format;
  
  return true;
}

bool VulkanGraphics::createRenderPass() {
  // Define a single color attachment
  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format = Vulkan::surfaceFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  
  // Define a Color Attachment Refrence at index 0
  VkAttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  
  // Define subpass using the Color Attachment Refrence
  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;
  
  // Create the Render Pass from the Color Attachment and Subpass
  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  
  // Create the Render Pass
  if (vkCreateRenderPass(Vulkan::device, &renderPassInfo, nullptr, &Vulkan::renderPass) != VK_SUCCESS) {
    cerr << "Error Creating Render Pass" << endl;
    return false;
  }
  return true;
}

bool VulkanGraphics::createCommandPool() {
  // Define the Command Pool Info
  VkCommandPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex = Vulkan::queueFamilyIndex;
  poolInfo.flags = 0; // Optional
  
  // Create the Command Pool
  if (vkCreateCommandPool(Vulkan::device, &poolInfo, nullptr, &Vulkan::commandPool) != VK_SUCCESS) {
    cerr << "Error Creating Command Pool" << endl;
    return false;
  }
  return true;
}

void VulkanGraphics::createCommandPools() {
  clearCommandPools();
  for (int i = 0; i < Vulkan::swapChainBuffers.size(); ++i)
    _commandPools.push_back(new VulkanCommandPool());
}

void VulkanGraphics::clearCommandPools() {
  for (auto &pool : _commandPools)
    delete pool;
  _commandPools.clear();
}
