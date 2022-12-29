#include <gtest/gtest.h>
#include <VulkanInstance.hpp>
#include <VulkanDevice.hpp>
#include <VulkanQueue.hpp>
#include <VulkanBuffer.hpp>
#include <iostream>
#include <cstring>
#include <vector>


using namespace std;

class TestVulkanBuffer : public ::testing::Test {
public:
  VulkanDevicePtr mDevice;
  VulkanQueuePtr mQueue;

protected:
  TestVulkanBuffer() {
    auto& instance = VulkanInstance::Get();
    instance.setApplicationName("Graphics Unit Tests");
    instance.setEngineName("No Engine");
    assert(instance.init());

    mDevice = VulkanInstance::Get().pickDevice();
    assert(mDevice != nullptr);

    mQueue = mDevice->createQueue(VK_QUEUE_TRANSFER_BIT);
    assert(mQueue != nullptr);
    assert(mDevice->init());
  }

  ~TestVulkanBuffer() {
    mQueue = nullptr;
    mDevice = nullptr;
    VulkanInstance::Get().destroy();
  }
};

TEST_F(TestVulkanBuffer, TestBufferMembers) {
  auto buffer = mDevice->createBuffer();
  ASSERT_TRUE(buffer != nullptr);
  EXPECT_EQ(buffer->size(), 0);
  EXPECT_EQ(buffer->frames(), 0);
  EXPECT_EQ(buffer->getVkBuffer(), VK_NULL_HANDLE);
  EXPECT_EQ(buffer->getVkBuffers().size(), 0);
  EXPECT_FALSE(buffer->allocated());
  EXPECT_ANY_THROW(buffer->getVmaAllocationInfo());
  EXPECT_ANY_THROW(buffer->getVkMemoryPropertyFlags());
  EXPECT_FALSE(buffer->isHostVisible());
  EXPECT_ANY_THROW(buffer->data());
}
