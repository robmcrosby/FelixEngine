#include <gtest/gtest.h>
#include <VulkanInstance.hpp>
#include <VulkanDevice.hpp>
#include <VulkanQueue.hpp>
#include <iostream>
#include <cstring>
#include <vector>


using namespace std;

class TestVulkanDevice : public ::testing::Test {
public:
  VulkanDevicePtr mDevice;

protected:
  TestVulkanDevice() {
    auto& instance = VulkanInstance::Get();
    instance.setApplicationName("Graphics Unit Tests");
    instance.setEngineName("No Engine");
    assert(instance.init());

    mDevice = VulkanInstance::Get().pickDevice();
    assert(mDevice != nullptr);
  }

  ~TestVulkanDevice() {
    mDevice = nullptr;
    VulkanInstance::Get().destroy();
  }
};


TEST_F(TestVulkanDevice, TestDeviceMembers) {
  //cout << mDevice->name() << endl;
  //cout << mDevice->type() << endl;

  EXPECT_GT(mDevice->name().size(), 0);
  EXPECT_GT(mDevice->type().size(), 0);
  EXPECT_EQ(mDevice->getVkInstance(), VulkanInstance::Get().getVkInstance());

  // Check pre-inital members
  EXPECT_NE(mDevice->getVkPhysicalDevice(), VK_NULL_HANDLE);
  EXPECT_EQ(mDevice->getVkDevice(), VK_NULL_HANDLE);
  EXPECT_EQ(mDevice->getVmaAllocator(), VK_NULL_HANDLE);

  // Initalize and check members
  EXPECT_TRUE(mDevice->init());
  EXPECT_NE(mDevice->getVkPhysicalDevice(), VK_NULL_HANDLE);
  EXPECT_NE(mDevice->getVkDevice(), VK_NULL_HANDLE);
  EXPECT_NE(mDevice->getVmaAllocator(), VK_NULL_HANDLE);

  // Destroy and check members
  mDevice->destroy();
  EXPECT_NE(mDevice->getVkPhysicalDevice(), VK_NULL_HANDLE);
  EXPECT_EQ(mDevice->getVkDevice(), VK_NULL_HANDLE);
  EXPECT_EQ(mDevice->getVmaAllocator(), VK_NULL_HANDLE);
}

TEST_F(TestVulkanDevice, TestExtensions) {
  EXPECT_FALSE(mDevice->addExtension("Non-Extension"));
  EXPECT_TRUE(mDevice->getEnabledExtensions().empty());

  const auto& extensions = mDevice->getAvailableExentions();
  if (!extensions.empty()) {
    // Add an Extension
    EXPECT_TRUE(mDevice->addExtension(extensions.front().extensionName));
    EXPECT_FALSE(mDevice->getEnabledExtensions().empty());

    // Attempt add after Initalization
    EXPECT_TRUE(mDevice->init());
    EXPECT_FALSE(mDevice->addExtension(extensions.back().extensionName));
  }

  mDevice->destroy();
  EXPECT_TRUE(mDevice->getEnabledExtensions().empty());
}

TEST_F(TestVulkanDevice, TestCreateMethods) {
  // Attempt to create items before initalization
  EXPECT_EQ(mDevice->createBuffer(), nullptr);
  EXPECT_EQ(mDevice->createSampler(), nullptr);
  EXPECT_EQ(mDevice->createLayout(), nullptr);
  EXPECT_EQ(mDevice->createLayoutSet(), nullptr);
  EXPECT_EQ(mDevice->createMesh(), nullptr);
  EXPECT_EQ(mDevice->createFrameBuffer(), nullptr);
  EXPECT_EQ(mDevice->createRenderPass(), nullptr);
  EXPECT_EQ(mDevice->createShader(VK_SHADER_STAGE_COMPUTE_BIT), nullptr);
  EXPECT_EQ(mDevice->createPipeline(), nullptr);
  EXPECT_EQ(mDevice->createSwapChain(), nullptr);
  EXPECT_EQ(mDevice->createFrameSync(), nullptr);

  // Create items after initalization
  EXPECT_TRUE(mDevice->init());
  EXPECT_NE(mDevice->createBuffer(), nullptr);
  EXPECT_NE(mDevice->createSampler(), nullptr);
  EXPECT_NE(mDevice->createLayout(), nullptr);
  EXPECT_NE(mDevice->createLayoutSet(), nullptr);
  EXPECT_NE(mDevice->createMesh(), nullptr);
  EXPECT_NE(mDevice->createFrameBuffer(), nullptr);
  EXPECT_NE(mDevice->createRenderPass(), nullptr);
  EXPECT_NE(mDevice->createShader(VK_SHADER_STAGE_COMPUTE_BIT), nullptr);
  EXPECT_NE(mDevice->createPipeline(), nullptr);
  EXPECT_NE(mDevice->createSwapChain(), nullptr);
  EXPECT_NE(mDevice->createFrameSync(), nullptr);
}

TEST_F(TestVulkanDevice, TestCreateQueue) {
  EXPECT_NE(mDevice->createQueue(VK_QUEUE_COMPUTE_BIT), nullptr);

  EXPECT_TRUE(mDevice->init());
  EXPECT_EQ(mDevice->createQueue(VK_QUEUE_COMPUTE_BIT), nullptr);
}
