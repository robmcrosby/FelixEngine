#include <gtest/gtest.h>
#include <VulkanInstance.hpp>
#include <VulkanDevice.hpp>
#include <VulkanQueue.hpp>
#include <VulkanCommand.hpp>
#include <iostream>
#include <cstring>
#include <memory>
#include <vector>


using namespace std;

class TestVulkanQueue : public ::testing::Test {
public:
  VulkanDevicePtr mDevice;

protected:
  TestVulkanQueue() {
    auto& instance = VulkanInstance::Get();
    instance.setApplicationName("Graphics Unit Tests");
    instance.setEngineName("No Engine");
    assert(instance.init());

    mDevice = VulkanInstance::Get().pickDevice();
    assert(mDevice != nullptr);
  }

  ~TestVulkanQueue() {
    mDevice = nullptr;
    VulkanInstance::Get().destroy();
  }
};

TEST_F(TestVulkanQueue, TestQueueMembers) {
  auto queue = mDevice->createQueue(VK_QUEUE_TRANSFER_BIT);

  EXPECT_FALSE(queue->initalized());
  EXPECT_EQ(queue->getVkQueue(), VK_NULL_HANDLE);
  EXPECT_EQ(queue->getVkCommandPool(), VK_NULL_HANDLE);

  ASSERT_TRUE(mDevice->init());
  EXPECT_TRUE(queue->initalized());
  EXPECT_NE(queue->getVkQueue(), VK_NULL_HANDLE);
  EXPECT_NE(queue->getVkCommandPool(), VK_NULL_HANDLE);

  mDevice->destroy();
  EXPECT_FALSE(queue->initalized());
  EXPECT_EQ(queue->getVkQueue(), VK_NULL_HANDLE);
  EXPECT_EQ(queue->getVkCommandPool(), VK_NULL_HANDLE);
}

TEST_F(TestVulkanQueue, TestCreateCommand) {
  auto queue = mDevice->createQueue(VK_QUEUE_TRANSFER_BIT);

  // Should not create commands before initalization
  EXPECT_EQ(queue->createCommand(), nullptr);
  EXPECT_EQ(queue->createCommand(3), nullptr);

  // Initalize and create commands
  ASSERT_TRUE(mDevice->init());
  auto command = queue->createCommand();
  ASSERT_NE(command, nullptr);
  EXPECT_EQ(command->frames(), 1);
  EXPECT_FALSE(command->isRecording());

  command = queue->createCommand(3);
  ASSERT_NE(command, nullptr);
  EXPECT_EQ(command->frames(), 3);
  EXPECT_FALSE(command->isRecording());
  command = nullptr;

  // Destroy queue
  mDevice->destroy();
  EXPECT_EQ(queue->createCommand(), nullptr);
  EXPECT_EQ(queue->createCommand(3), nullptr);
}

TEST_F(TestVulkanQueue, TestBeginSingleCommand) {
  auto queue = mDevice->createQueue(VK_QUEUE_TRANSFER_BIT);

  // Should not create commands before initalization
  EXPECT_EQ(queue->beginSingleCommand(), nullptr);

  // Initalize and create command
  ASSERT_TRUE(mDevice->init());
  auto command = queue->beginSingleCommand();
  ASSERT_NE(command, nullptr);
  EXPECT_EQ(command->frames(), 1);
  EXPECT_TRUE(command->isRecording());
  command->end();
  command = nullptr;

  // Destroy queue
  mDevice->destroy();
  EXPECT_EQ(queue->beginSingleCommand(), nullptr);
}
