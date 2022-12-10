#include <VulkanIncludes.hpp>
#include <fstream>

using namespace std;


VulkanCommand::VulkanCommand(VulkanQueue* queue, int frames):
  mQueue(queue),
  mVkCommandPool(VK_NULL_HANDLE),
  mRecordingBuffer(VK_NULL_HANDLE),
  mRecordingFrame(0) {
  alloc(static_cast<uint32_t>(frames));
}

VulkanCommand::~VulkanCommand() {
  destroy();
}

bool VulkanCommand::alloc(uint32_t frames) {
  mVkCommandPool = mQueue->getVkCommandPool();
  mVkCommandBuffers.resize(frames);

  VkCommandBufferAllocateInfo allocInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, 0};
  allocInfo.commandPool = mVkCommandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
  allocInfo.commandBufferCount = frames;

  VkDevice device = mQueue->getDevice()->getVkDevice();
  if (vkAllocateCommandBuffers(device, &allocInfo, mVkCommandBuffers.data()) != VK_SUCCESS) {
    cerr << "Error Allocating Command Buffer" << endl;
    mVkCommandBuffers.clear();
  }
  return !mVkCommandBuffers.empty();
}

bool VulkanCommand::begin(int frame, VkCommandBufferUsageFlags usageFlags) {
  mRecordingBuffer = mVkCommandBuffers.at(frame);
  mRecordingFrame = frame;

  VkCommandBufferBeginInfo beginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, 0};
  beginInfo.flags = usageFlags;
  beginInfo.pInheritanceInfo = 0;

  return mRecordingBuffer != VK_NULL_HANDLE && vkBeginCommandBuffer(mRecordingBuffer, &beginInfo) == VK_SUCCESS;
}

void VulkanCommand::end() {
  if (mRecordingBuffer != VK_NULL_HANDLE) {
    vkEndCommandBuffer(mRecordingBuffer);
    mRecordingBuffer = VK_NULL_HANDLE;
    mRecordingFrame = 0;
  }
}

void VulkanCommand::submit(int frame) {
  auto commandBuffer = mVkCommandBuffers.at(frame);

  if (commandBuffer != VK_NULL_HANDLE) {
    VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO, 0};
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = 0;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer,
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
  if (mRecordingBuffer != VK_NULL_HANDLE) {
    vkCmdBindPipeline(
      mRecordingBuffer,
      VK_PIPELINE_BIND_POINT_COMPUTE,
      pipeline->getVkPipeline()
    );
  }
}

void VulkanCommand::bind(VulkanLayoutSetPtr layoutSet) {
  if (mRecordingBuffer != VK_NULL_HANDLE) {
    VkDescriptorSets descriptorSets;
    layoutSet->getVkDescriptorSets(descriptorSets, 0);
    vkCmdBindDescriptorSets(
      mRecordingBuffer,
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
  if (mRecordingBuffer != VK_NULL_HANDLE) {
    vkCmdBindPipeline(
      mRecordingBuffer,
      VK_PIPELINE_BIND_POINT_COMPUTE,
      pipeline->getVkPipeline(layoutSet)
    );

    VkDescriptorSets descriptorSets;
    layoutSet->getVkDescriptorSets(descriptorSets, 0);
    vkCmdBindDescriptorSets(
      mRecordingBuffer,
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
  if (mRecordingBuffer != VK_NULL_HANDLE)
    vkCmdDispatch(mRecordingBuffer, x, y, z);
}

void VulkanCommand::beginRenderPass(VulkanRenderPassPtr renderPass) {
  if (mRecordingBuffer != VK_NULL_HANDLE) {
    vector<VkClearValue> clearValues(renderPass->getColorCount());
    for (int i = 0; i < clearValues.size(); ++i) {
      clearValues.at(i).color.float32[0] = 0.0f;
      clearValues.at(i).color.float32[1] = 0.0f;
      clearValues.at(i).color.float32[2] = 0.0f;
      clearValues.at(i).color.float32[3] = 1.0f;
    }

    VkRenderPassBeginInfo renderPassInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    renderPassInfo.renderPass = renderPass->getVkRenderPass(mRecordingFrame);
    renderPassInfo.framebuffer = renderPass->getVkFramebuffer(mRecordingFrame);
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = renderPass->getExtent();
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(mRecordingBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
  }
}

void VulkanCommand::endRenderPass() {
  if (mRecordingBuffer != VK_NULL_HANDLE)
    vkCmdEndRenderPass(mRecordingBuffer);
}

void VulkanCommand::bind(
  VulkanPipelinePtr   pipeline,
  VulkanRenderPassPtr renderPass,
  VulkanMeshPtr       mesh,
  VulkanLayoutSetPtr  layoutSet
) {
  if (mRecordingBuffer != VK_NULL_HANDLE) {
    vkCmdBindPipeline(
      mRecordingBuffer,
      VK_PIPELINE_BIND_POINT_GRAPHICS,
      pipeline->getVkPipeline(renderPass, mesh, layoutSet)
    );

    if (mesh != nullptr) {
      VkDeviceSizes offsets;
      VkBuffers     buffers;
      mesh->getVertexBufferOffsets(offsets);
      mesh->getVertexBuffers(buffers);

	    vkCmdBindVertexBuffers(
        mRecordingBuffer,
        0, static_cast<uint32_t>(buffers.size()),
        buffers.data(), offsets.data()
      );
    }

    if (layoutSet != nullptr) {
      VkDescriptorSets descriptorSets;
      layoutSet->getVkDescriptorSets(descriptorSets, mRecordingFrame);
      vkCmdBindDescriptorSets(
        mRecordingBuffer,
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
  if (mRecordingBuffer != VK_NULL_HANDLE) {
    vkCmdDraw(
      mRecordingBuffer,
      vertexCount,
      instances,
      0, 0
    );
  }
}

void VulkanCommand::draw(VulkanBufferPtr indexBuffer, uint32_t instances) {
  if (mRecordingBuffer != VK_NULL_HANDLE && indexBuffer != nullptr) {
    vkCmdBindIndexBuffer(
      mRecordingBuffer,
      indexBuffer->getVkBuffer(),
      0, VK_INDEX_TYPE_UINT32
    );

    uint32_t count  = static_cast<uint32_t>(indexBuffer->size() / sizeof(uint32_t));
    vkCmdDrawIndexed(
      mRecordingBuffer,
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
  if (mRecordingBuffer != VK_NULL_HANDLE)
    image->transition(mRecordingBuffer, newImageLayout, dstAccessMask, dstStageMask);
}

void VulkanCommand::copyBufferToImage(VulkanBufferPtr buffer, VulkanImagePtr image) {
  if (mRecordingBuffer != VK_NULL_HANDLE)
    image->copyFromBuffer(mRecordingBuffer, buffer);
}

void VulkanCommand::copyImageToBuffer(VulkanImagePtr image, VulkanBufferPtr buffer) {
  if (mRecordingBuffer != VK_NULL_HANDLE)
    image->copyToBuffer(mRecordingBuffer, buffer);
}

void VulkanCommand::copyBufferToBuffer(VulkanBufferPtr src, VulkanBufferPtr dst) {
  if (mRecordingBuffer != VK_NULL_HANDLE)
    src->copyToBuffer(mRecordingBuffer, dst);
}

void VulkanCommand::free() {
  if (!mVkCommandBuffers.empty()) {
    VkDevice device = mQueue->getDevice()->getVkDevice();
    vkFreeCommandBuffers(device, mVkCommandPool, 1, mVkCommandBuffers.data());
    mVkCommandBuffers.clear();
    mVkCommandPool = VK_NULL_HANDLE;
  }
}

void VulkanCommand::destroy() {
  free();
}
