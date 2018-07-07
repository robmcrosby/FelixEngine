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
#include "VulkanBufferPool.h"
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
  VulkanShaderProgram *shader = static_cast<VulkanShaderProgram*>(firstTask.shader.get());
  VulkanFrameBuffer *frame = static_cast<VulkanFrameBuffer*>(firstTask.frame.get());
  VkPipeline pipeline = frame->getVkPipelineForTask(firstTask);
  
  int bufferIndex = 0;
  for (GraphicTask &task : pass) {
    VulkanVertexMesh *mesh = static_cast<VulkanVertexMesh*>(task.mesh.get());
    VulkanShaderProgram *shader = static_cast<VulkanShaderProgram*>(task.shader.get());
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
    renderPassInfo.renderPass = frame->getRenderPass(task);
    renderPassInfo.framebuffer = frame->getFrameBuffer(task);
    // Define the Render Extents
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = Vulkan::swapChainExtent;
    
    // Define the Clear Values
    int totalColorAttachments = frame->totalColorAttachements();
    vector<VkClearValue> clearValues(totalColorAttachments);
    for (int i = 0; i < totalColorAttachments; ++i)
      memcpy(&clearValues[i], task.colorActions[i].clearColor.ptr(), sizeof(vec4));
    if (frame->hasDepthBuffer()) {
      VkClearValue value;
      value.depthStencil.depth = task.depthStencilAction.clearColor.r;
      value.depthStencil.stencil = (int)task.depthStencilAction.clearColor.g;
      clearValues.push_back(value);
    }
    
    renderPassInfo.clearValueCount = (uint32_t)clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();
    
    // Begin the Render Pass
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    // Bind the Graphics Pipeline
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    
    uint32_t vertexBufferCount = mesh->getVertexBufferCount();
    VkBuffer *vertexBuffers = mesh->getVertexBuffers();
    VkDeviceSize *offsets = mesh->getVertexOffsets();
    vkCmdBindVertexBuffers(commandBuffer, 0, vertexBufferCount, vertexBuffers, offsets);
    
    bool texturesAdded = false;
    vector<VkDescriptorSet> descriptorSets;
    for (auto &uniformMap : task.uniforms) {
      if (!texturesAdded && task.textures && !VulkanBufferPool::isVertexUniform(shader, *uniformMap)) {
        VulkanBufferPool *bufferPool = static_cast<VulkanBufferPool*>(task.textures->bufferPool().get());
        bufferPool->addDescriptorSet(descriptorSets, shader, *task.textures, *uniformMap);
        texturesAdded = true;
      }
      else {
        VulkanBufferPool *bufferPool = static_cast<VulkanBufferPool*>(uniformMap->bufferPool().get());
        bufferPool->addDescriptorSet(descriptorSets, shader, *uniformMap);
      }
    }
    if (task.textures && !texturesAdded) {
      VulkanBufferPool *bufferPool = static_cast<VulkanBufferPool*>(task.textures->bufferPool().get());
      bufferPool->addDescriptorSet(descriptorSets, shader, *task.textures);
    }
    uint32_t count = (uint32_t)descriptorSets.size();
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, frame->getPipelineLayout(), 0, count, descriptorSets.data(), 0, nullptr);
    
    // Draw the mesh
    mesh->drawToCommandBuffer(commandBuffer);
    
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
