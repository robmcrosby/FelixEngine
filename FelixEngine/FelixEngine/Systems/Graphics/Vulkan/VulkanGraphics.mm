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
  //assert(createPipeline());
  assert(createFrameBuffers());
  assert(createCommandPool());
  //assert(createCommandBuffers());
  assert(Vulkan::createSemaphores());
  
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
  vkAcquireNextImageKHR(Vulkan::device, Vulkan::swapChain, std::numeric_limits<uint64_t>::max(), Vulkan::imageAvailableSemaphore, VK_NULL_HANDLE, &_imageIndex);
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
  for (RenderPass &renderPass : _renderPasses)
    submitRenderPass(renderPass);
  
  
  
  VkSemaphore signalSemaphores[] = {Vulkan::renderFinishedSemaphore};
  VkPresentInfoKHR presentInfo = {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;
  
  VkSwapchainKHR swapChains[] = {Vulkan::swapChain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  
  presentInfo.pImageIndices = &_imageIndex;
  
  vkQueuePresentKHR(Vulkan::presentQueue, &presentInfo);
  
  //drawFrame();
}

void VulkanGraphics::submitRenderPass(RenderPass &renderPass) {
  GraphicTask &firstTask = renderPass.front();
  VulkanFrameBuffer   *frame  = static_cast<VulkanFrameBuffer*>(firstTask.frame.get());
//  MetalShaderProgram *shader = static_cast<MetalShaderProgram*>(task.shader.get());
//  MetalVertexMesh    *mesh   = static_cast<MetalVertexMesh*>(task.mesh.get());
  
  
  
  VkPipeline pipeline = frame->getVkPipelineForTask(firstTask);
  
  
  // Define the Command Buffer Info
  VkCommandBufferAllocateInfo commandBufferInfo = {};
  commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferInfo.commandPool = Vulkan::commandPool;
  commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferInfo.commandBufferCount = 1;
  
  // Create the Command Buffer
  VkCommandBuffer commandBuffer;
  if (vkAllocateCommandBuffers(Vulkan::device, &commandBufferInfo, &commandBuffer) != VK_SUCCESS) {
    cerr << "Error Creating Command Buffers" << endl;
    assert(false);
  }
  
  // Record each of the Command Buffers
  //for (size_t i = 0; i < Vulkan::commandBuffers.size(); i++) {
    // Define the Begin Info
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    beginInfo.pInheritanceInfo = nullptr; // Optional
    
    // Begin Recording the Command Buffer
    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
      cerr << "Failed to begin recording command buffer" << endl;
      assert(false);
    }
    
    // Define the Render Pass Info
    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = Vulkan::renderPass;
    renderPassInfo.framebuffer = Vulkan::swapChainFrameBuffers[_imageIndex];
    // Define the Render Extents
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = Vulkan::swapChainExtent;
    // Define the Clear Color
    VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    
    // Begin the Render Pass
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    // Bind the Graphics Pipeline
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    
    // Draw the triangle
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
    
    // Finish the Render Pass
    vkCmdEndRenderPass(commandBuffer);
    
    // End Recording the Command Buffer
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
      cerr << "Error Recording the Command Buffer" << endl;
      assert(false);
    }
  //}
  
  
  
  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  
  VkSemaphore waitSemaphores[] = {Vulkan::imageAvailableSemaphore};
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;
  
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;
  
  VkSemaphore signalSemaphores[] = {Vulkan::renderFinishedSemaphore};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;
  
  if (vkQueueSubmit(Vulkan::graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
    cerr << "failed to submit draw command buffer!" << endl;
    assert(false);
  }
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

bool VulkanGraphics::createFrameBuffers() {
  Vulkan::swapChainFrameBuffers.resize(Vulkan::swapChainBuffers.size());
  
  for (size_t i = 0; i < Vulkan::swapChainBuffers.size(); i++) {
    VkImageView attachments[] = {Vulkan::swapChainBuffers[i].view};
    
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = Vulkan::renderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = Vulkan::swapChainExtent.width;
    framebufferInfo.height = Vulkan::swapChainExtent.height;
    framebufferInfo.layers = 1;
    
    if (vkCreateFramebuffer(Vulkan::device, &framebufferInfo, nullptr, &Vulkan::swapChainFrameBuffers[i]) != VK_SUCCESS) {
      cerr << "Error Creating FrameBuffer" << endl;
      return false;
    }
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
