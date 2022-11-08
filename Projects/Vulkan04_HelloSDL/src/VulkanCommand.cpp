#include "VulkanIncludes.hpp"
#include <fstream>

using namespace std;


VulkanCommand::VulkanCommand(VulkanQueue* queue):
  mQueue(queue),
  mVkCommandPool(VK_NULL_HANDLE),
  mVkCommandBuffer(VK_NULL_HANDLE) {
  alloc();
}

VulkanCommand::~VulkanCommand() {
  destroy();
}

bool VulkanCommand::alloc() {
  if (mVkCommandBuffer == VK_NULL_HANDLE) {
    mVkCommandPool = mQueue->getVkCommandPool();

    VkCommandBufferAllocateInfo allocInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, 0};
    allocInfo.commandPool = mVkCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    allocInfo.commandBufferCount = 1;

    VkDevice device = mQueue->getDevice()->getVkDevice();
    if (vkAllocateCommandBuffers(device, &allocInfo, &mVkCommandBuffer) != VK_SUCCESS) {
      cerr << "Error Allocating Command Buffer" << endl;
      mVkCommandBuffer = VK_NULL_HANDLE;
    }
  }
  return mVkCommandBuffer != VK_NULL_HANDLE;
}

bool VulkanCommand::begin() {
  VkCommandBufferBeginInfo beginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, 0};
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  beginInfo.pInheritanceInfo = 0;

  return mVkCommandBuffer != VK_NULL_HANDLE && vkBeginCommandBuffer(mVkCommandBuffer, &beginInfo) == VK_SUCCESS;
}

void VulkanCommand::end() {
  if (mVkCommandBuffer != VK_NULL_HANDLE)
    vkEndCommandBuffer(mVkCommandBuffer);
}

void VulkanCommand::submit() {
  if (mVkCommandBuffer != VK_NULL_HANDLE) {
    VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO, 0};
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = 0;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &mVkCommandBuffer,
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = 0;

    if (vkQueueSubmit(mQueue->getVkQueue(), 1, &submitInfo, 0) != VK_SUCCESS)
      cerr << "Error Submiting Command to Queue" << endl;
  }
}

void VulkanCommand::endSingle() {
  end();
  submit();
}

void VulkanCommand::bind(VulkanPipelinePtr pipeline) {
  if (mVkCommandBuffer != VK_NULL_HANDLE) {
    vkCmdBindPipeline(
      mVkCommandBuffer,
      VK_PIPELINE_BIND_POINT_COMPUTE,
      pipeline->getVkPipeline()
    );
  }
}

void VulkanCommand::bind(VulkanLayoutSetPtr layoutSet) {
  if (mVkCommandBuffer != VK_NULL_HANDLE) {
    VkDescriptorSets descriptorSets;
    layoutSet->getVkDescriptorSets(descriptorSets);
    vkCmdBindDescriptorSets(
      mVkCommandBuffer,
      VK_PIPELINE_BIND_POINT_COMPUTE,
      layoutSet->getVkPipelineLayout(),
      0,
      static_cast<uint32_t>(descriptorSets.size()),
      descriptorSets.data(),
      0, 0
    );
  }
}

void VulkanCommand::bind(VulkanPipelinePtr pipeline, VulkanLayoutSetPtr layoutSet) {
  if (mVkCommandBuffer != VK_NULL_HANDLE) {
    vkCmdBindPipeline(
      mVkCommandBuffer,
      VK_PIPELINE_BIND_POINT_COMPUTE,
      pipeline->getVkPipeline(layoutSet)
    );

    VkDescriptorSets descriptorSets;
    layoutSet->getVkDescriptorSets(descriptorSets);
    vkCmdBindDescriptorSets(
      mVkCommandBuffer,
      VK_PIPELINE_BIND_POINT_COMPUTE,
      pipeline->getVkPipelineLayout(),
      0,
      static_cast<uint32_t>(descriptorSets.size()),
      descriptorSets.data(),
      0, 0
    );
  }
}

void VulkanCommand::dispatch(uint32_t x, uint32_t y, uint32_t z) {
  if (mVkCommandBuffer != VK_NULL_HANDLE)
    vkCmdDispatch(mVkCommandBuffer, x, y, z);
}

void VulkanCommand::beginRenderPass(VulkanRenderPassPtr renderPass) {
  if (mVkCommandBuffer != VK_NULL_HANDLE) {
    vector<VkClearValue> clearValues(renderPass->getColorCount());
    for (int i = 0; i < clearValues.size(); ++i) {
      clearValues.at(i).color.float32[0] = 0.0f;
      clearValues.at(i).color.float32[1] = 0.0f;
      clearValues.at(i).color.float32[2] = 0.0f;
      clearValues.at(i).color.float32[3] = 1.0f;
    }

    VkRenderPassBeginInfo renderPassInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    renderPassInfo.renderPass = renderPass->getVkRenderPass();
    renderPassInfo.framebuffer = renderPass->getVkFramebuffer();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = renderPass->getExtent();
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(mVkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
  }
}

void VulkanCommand::endRenderPass() {
  if (mVkCommandBuffer != VK_NULL_HANDLE)
    vkCmdEndRenderPass(mVkCommandBuffer);
}

void VulkanCommand::bind(
  VulkanPipelinePtr   pipeline,
  VulkanRenderPassPtr renderPass,
  VulkanMeshPtr       mesh,
  VulkanLayoutSetPtr  layoutSet
) {
  if (mVkCommandBuffer != VK_NULL_HANDLE) {
    vkCmdBindPipeline(
      mVkCommandBuffer,
      VK_PIPELINE_BIND_POINT_GRAPHICS,
      pipeline->getVkPipeline(renderPass, mesh, layoutSet)
    );

    if (mesh != nullptr) {
      VkDeviceSizes offsets;
      VkBuffers     buffers;
      mesh->getVertexBufferOffsets(offsets);
      mesh->getVertexBuffers(buffers);

	    vkCmdBindVertexBuffers(
        mVkCommandBuffer,
        0, static_cast<uint32_t>(buffers.size()),
        buffers.data(), offsets.data()
      );
    }

    if (layoutSet != nullptr) {
      VkDescriptorSets descriptorSets;
      layoutSet->getVkDescriptorSets(descriptorSets);
      vkCmdBindDescriptorSets(
        mVkCommandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline->getVkPipelineLayout(),
        0,
        static_cast<uint32_t>(descriptorSets.size()),
        descriptorSets.data(),
        0, 0
      );
    }
  }
}

void VulkanCommand::draw(uint32_t vertexCount, uint32_t instances) {
  if (mVkCommandBuffer != VK_NULL_HANDLE) {
    vkCmdDraw(
      mVkCommandBuffer,
      vertexCount,
      instances,
      0, 0
    );
  }
}

void VulkanCommand::draw(VulkanBufferPtr indexBuffer, uint32_t instances) {
  if (mVkCommandBuffer != VK_NULL_HANDLE && indexBuffer != nullptr) {
    vkCmdBindIndexBuffer(
      mVkCommandBuffer,
      indexBuffer->getVkBuffer(),
      0, VK_INDEX_TYPE_UINT32
    );

    uint32_t count  = static_cast<uint32_t>(indexBuffer->size() / sizeof(uint32_t));
    vkCmdDrawIndexed(
      mVkCommandBuffer,
      count,
      instances,
      0, 0, 0
    );
  }
}

void VulkanCommand::draw(VulkanMeshPtr mesh, uint32_t instances) {
  if (mesh->getIndexCount() > 0)
    draw(mesh->getIndexBuffer(), instances);
  else
    draw(mesh->getVertexCount(), instances);
}

void VulkanCommand::transition(
  VulkanImagePtr image,
  VkImageLayout  newImageLayout,
  VkAccessFlags  dstAccessMask,
  VkPipelineStageFlags dstStageMask
) {
  if (mVkCommandBuffer != VK_NULL_HANDLE)
    image->transition(mVkCommandBuffer, newImageLayout, dstAccessMask, dstStageMask);
}

void VulkanCommand::copyBufferToImage(VulkanBufferPtr buffer, VulkanImagePtr image) {
  if (mVkCommandBuffer != VK_NULL_HANDLE)
    image->copyFromBuffer(mVkCommandBuffer, buffer);
}

void VulkanCommand::copyImageToBuffer(VulkanImagePtr image, VulkanBufferPtr buffer) {
  if (mVkCommandBuffer != VK_NULL_HANDLE)
    image->copyToBuffer(mVkCommandBuffer, buffer);
}

void VulkanCommand::copyBufferToBuffer(VulkanBufferPtr src, VulkanBufferPtr dst) {
  if (mVkCommandBuffer != VK_NULL_HANDLE)
    src->copyToBuffer(mVkCommandBuffer, dst);
}

void VulkanCommand::free() {
  if (mVkCommandBuffer != VK_NULL_HANDLE) {
    VkDevice device = mQueue->getDevice()->getVkDevice();
    vkFreeCommandBuffers(device, mVkCommandPool, 1, &mVkCommandBuffer);
    mVkCommandPool = VK_NULL_HANDLE;
    mVkCommandBuffer = VK_NULL_HANDLE;
  }
}

void VulkanCommand::destroy() {
  free();
}
