//
//  VulkanCommandBuffer.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 5/11/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "VulkanCommandBuffer.h"
#include "Vulkan.h"
#include "GraphicTask.h"

#include "VulkanFrameBuffer.h"
#include "VulkanShaderProgram.h"
#include "VulkanVertexMesh.h"
#include "VulkanUniformBuffer.h"
#include "VulkanTextureBuffer.h"
#include "VulkanCommandPool.h"


using namespace std;
using namespace fx;



VulkanCommandBuffer::VulkanCommandBuffer() {
  
}

VulkanCommandBuffer::~VulkanCommandBuffer() {
  clearBuffers();
}

void VulkanCommandBuffer::submitRenderPass(RenderPass &pass) {
  resizeBuffers(pass.size());
  
  // Get the Pipeline from the first task.
  GraphicTask &firstTask = pass.front();
  VulkanFrameBuffer *frame = static_cast<VulkanFrameBuffer*>(firstTask.frame.get());
  VkPipeline pipeline = frame->getVkPipelineForTask(firstTask);
  
  int bufferIndex = 0;
  for (GraphicTask &task : pass) {
    VkCommandBuffer &commandBuffer = _commandBuffers[bufferIndex];
    
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
    renderPassInfo.framebuffer = Vulkan::swapChainFrameBuffers[Vulkan::currentSwapBuffer];
    // Define the Render Extents
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = Vulkan::swapChainExtent;
    // Define the Clear Color
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = (VkClearValue*)task.colorActions[0].clearColor.ptr();
    
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
    
    // Increment the buffer index
    ++bufferIndex;
  }
  
  
  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  
  VkSemaphore waitSemaphores[] = {Vulkan::imageAvailableSemaphore};
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;
  
  uint32_t count = (uint32_t)_commandBuffers.size();
  submitInfo.commandBufferCount = count;
  submitInfo.pCommandBuffers = _commandBuffers.data();
  
  VkSemaphore signalSemaphores[] = {Vulkan::renderFinishedSemaphore};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;
  
  if (vkQueueSubmit(Vulkan::graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
    cerr << "failed to submit draw command buffer!" << endl;
    assert(false);
  }
}

void VulkanCommandBuffer::resizeBuffers(size_t size) {
  if (_commandBuffers.size() < size) {
    clearBuffers();
    
    // Define the Command Buffer Info
    VkCommandBufferAllocateInfo commandBufferInfo = {};
    commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferInfo.commandPool = Vulkan::commandPool;
    commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferInfo.commandBufferCount = (uint32_t)size;
    
    _commandBuffers.resize(size);
    if (vkAllocateCommandBuffers(Vulkan::device, &commandBufferInfo, _commandBuffers.data()) != VK_SUCCESS) {
      cerr << "Error Creating Command Buffers" << endl;
      assert(false);
    }
  }
}

void VulkanCommandBuffer::clearBuffers() {
  if (_commandBuffers.size() > 0) {
    uint32_t size = (uint32_t)_commandBuffers.size();
    vkFreeCommandBuffers(Vulkan::device, Vulkan::commandPool, size, _commandBuffers.data());
    _commandBuffers.clear();
  }
}
