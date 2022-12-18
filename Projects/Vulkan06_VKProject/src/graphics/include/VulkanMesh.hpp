#include <VulkanTypes.hpp>

#ifndef VulkanMesh_hpp
#define VulkanMesh_hpp


struct VertexBuffer {
  VulkanBufferPtr buffer;
  VkDeviceSize    offset;
  VkVertexInputBindingDescription description;
  VkVertexInputAttributeDescriptions attributes;
};
typedef std::vector<VertexBuffer> VertexBuffers;


class VulkanMesh {
private:
  VulkanDevice*   mDevice;
  VertexBuffers   mVertexBuffers;
  VulkanBufferPtr mIndexBuffer;
  uint32_t        mVertexCount;
  uint32_t        mIndexCount;
  VkPrimitiveTopology mPrimitiveTopology;

public:
  VulkanMesh(VulkanDevice* device);
  ~VulkanMesh();

  bool addBuffer(VulkanQueuePtr queue, const std::vector<float>& vertices, int vertexSize);
  bool addBuffer(
    VulkanQueuePtr queue,
    const void* data,
    VkDeviceSize size,
    uint32_t stride
  );
  void addAttribute(int buffer, int location, int size, int offset);
  bool setIndexBuffer(VulkanQueuePtr queue, const std::vector<uint32_t>& indices);
  void setTopology(VkPrimitiveTopology topology) {mPrimitiveTopology = topology;}

  void getVertexBindings(VkVertexInputBindingDescriptions& bindings);
  void getVertexAttributes(VkVertexInputAttributeDescriptions& attributes);
  VkPrimitiveTopology getVkPrimitiveTopology() const {return mPrimitiveTopology;}

  uint32_t getVertexCount() const {return mVertexCount;}
  uint32_t getIndexCount() const {return mIndexCount;}
  VulkanBufferPtr getIndexBuffer() {return mIndexBuffer;}

  void getVertexBufferOffsets(VkDeviceSizes& offsets);
  void getVertexBuffers(VkBuffers& buffers);

  void destroy();

  template <typename T>
  bool addBuffer(VulkanQueuePtr queue, const std::vector<T>& src) {
    return addBuffer(queue, src.data(), src.size() * sizeof(T), sizeof(T));
  }

private:
  VkFormat getVertexFormat(int size);
};


#endif /* VulkanMesh_hpp */
