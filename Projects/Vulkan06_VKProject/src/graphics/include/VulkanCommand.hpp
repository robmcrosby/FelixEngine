#include <VulkanTypes.hpp>

#ifndef VulkanCommand_hpp
#define VulkanCommand_hpp


class VulkanCommand {
private:
  VulkanQueue*     mQueue;
  VkCommandPool    mVkCommandPool;
  VkCommandBuffers mVkCommandBuffers;
  VkCommandBuffer  mRecordingBuffer;
  int              mRecordingFrame;

public:
  VulkanCommand(VulkanQueue* queue, int frames);
  ~VulkanCommand();

  VkCommandBuffer getVkCommandBuffer(int frame = 0) const {
    return mVkCommandBuffers.at(frame);
  }

  int frames() const {return static_cast<int>(mVkCommandBuffers.size());}
  bool isRecording() const {return mRecordingBuffer != VK_NULL_HANDLE;}

  bool begin(int frame = 0, VkCommandBufferUsageFlags usageFlags = 0);
  void end();
  void submit(int frame = 0);
  void endSingle();

  void bind(VulkanPipelinePtr pipeline);
  void bind(VulkanLayoutSetPtr layoutSet);
  void bind(VulkanPipelinePtr pipeline, VulkanLayoutSetPtr layoutSet);
  void dispatch(uint32_t x, uint32_t y = 1, uint32_t z = 1);

  void beginRenderPass(VulkanRenderPassPtr renderPass);
  void endRenderPass();

  void bind(
    VulkanPipelinePtr   pipeline,
    VulkanRenderPassPtr renderPass,
    VulkanMeshPtr       mesh        = nullptr,
    VulkanLayoutSetPtr  layoutSet   = nullptr
  );
  void draw(uint32_t vertexCount, uint32_t instances = 1);
  void draw(VulkanBufferPtr indexBuffer, uint32_t instances = 1);
  void draw(VulkanMeshPtr mesh, uint32_t instances = 1);

  void transition(
    VulkanImagePtr image,
    VkImageLayout  newImageLayout,
    VkAccessFlags  dstAccessMask,
    VkPipelineStageFlags dstStageMask
  );
  void copyBufferToImage(VulkanBufferPtr buffer, VulkanImagePtr image);
  void copyImageToBuffer(VulkanImagePtr image, VulkanBufferPtr buffer);
  void copyBufferToBuffer(VulkanBufferPtr src, VulkanBufferPtr dst);

  bool alloc(uint32_t frames);
  void free();

  void destroy();
};


#endif /* VulkanCommand_hpp */
