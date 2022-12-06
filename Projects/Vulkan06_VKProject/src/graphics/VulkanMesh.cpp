#include "VulkanIncludes.hpp"
#include <cstring>


using namespace std;


VulkanMesh::VulkanMesh(VulkanDevice* device):
  mDevice(device),
  mVertexCount(0),
  mIndexCount(0),
  mPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST) {

}

VulkanMesh::~VulkanMesh() {
  destroy();
}

bool VulkanMesh::addBuffer(VulkanQueuePtr queue, const vector<float>& vertices, int vertexSize) {
  return addBuffer(
    queue,
    vertices.data(),
    vertices.size() * sizeof(float),
    vertexSize * sizeof(float)
  );
}

bool VulkanMesh::addBuffer(
  VulkanQueuePtr queue,
  const void* data,
  VkDeviceSize size,
  uint32_t stride
) {
  mVertexCount = size / stride;

  VertexBuffer vertexBuffer;
  vertexBuffer.description.binding = static_cast<uint32_t>(mVertexBuffers.size());
  vertexBuffer.description.stride = stride;
  vertexBuffer.description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  vertexBuffer.offset = 0;
  vertexBuffer.buffer = mDevice->createBuffer();
  vertexBuffer.buffer->setUsage(
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
    VK_BUFFER_USAGE_TRANSFER_DST_BIT
  );
  vertexBuffer.buffer->setCreateFlags(
    VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
    VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
    VMA_ALLOCATION_CREATE_MAPPED_BIT
  );

  if (!vertexBuffer.buffer->load(queue, data, size))
    return false;

  mVertexBuffers.push_back(vertexBuffer);
  return true;
}

void VulkanMesh::addAttribute(int binding, int location, int size, int offset) {
  VkVertexInputAttributeDescription attribute;
  attribute.binding  = static_cast<uint32_t>(binding);
  attribute.location = static_cast<uint32_t>(location);
  attribute.format   = getVertexFormat(size);
  attribute.offset   = static_cast<uint32_t>(offset * sizeof(float));
  mVertexBuffers.at(binding).attributes.push_back(attribute);
}

bool VulkanMesh::setIndexBuffer(VulkanQueuePtr queue, const vector<uint32_t>& indices) {
  mIndexCount = static_cast<uint32_t>(indices.size());

  mIndexBuffer = mDevice->createBuffer();
  mIndexBuffer->setUsage(
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
    VK_BUFFER_USAGE_TRANSFER_DST_BIT
  );
  mIndexBuffer->setCreateFlags(
    VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
    VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
    VMA_ALLOCATION_CREATE_MAPPED_BIT
  );

  VkDeviceSize size = indices.size() * sizeof(float);
  if (!mIndexBuffer->alloc(size)) {
    cerr << "Error Allocating Index Buffer" << endl;
    return false;
  }

  if (mIndexBuffer->isHostVisible())
    memcpy(mIndexBuffer->data(), indices.data(), size);
  else {
    auto staging = mDevice->createBuffer();
    staging->setUsage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    staging->setCreateFlags(
      VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
      VMA_ALLOCATION_CREATE_MAPPED_BIT
    );
    if (staging->alloc(size)) {
      memcpy(staging->data(), indices.data(), size);
      queue->copyBufferToBuffer(staging, mIndexBuffer);
    }
  }
  return true;
}

void VulkanMesh::getVertexBindings(VkVertexInputBindingDescriptions& bindings) {
  for (const auto& buffer : mVertexBuffers)
    bindings.push_back(buffer.description);
}

void VulkanMesh::getVertexAttributes(VkVertexInputAttributeDescriptions& attributes) {
  for (const auto& buffer : mVertexBuffers)
    attributes.insert(attributes.end(), buffer.attributes.begin(), buffer.attributes.end());
}

void VulkanMesh::getVertexBufferOffsets(VkDeviceSizes& offsets) {
  for (const auto& buffer : mVertexBuffers)
    offsets.push_back(buffer.offset);
}

void VulkanMesh::getVertexBuffers(VkBuffers& buffers) {
  for (const auto& buffer : mVertexBuffers)
    buffers.push_back(buffer.buffer->getVkBuffer());
}

void VulkanMesh::destroy() {
  mVertexBuffers.clear();
}

VkFormat VulkanMesh::getVertexFormat(int size) {
  if (size == 4)
    return VK_FORMAT_R32G32B32A32_SFLOAT;
  if (size == 3)
    return VK_FORMAT_R32G32B32_SFLOAT;
  if (size == 2)
    return VK_FORMAT_R32G32_SFLOAT;
  return VK_FORMAT_R32_SFLOAT;
}
