#include <gtest/gtest.h>
#include <VulkanInstance.hpp>
#include <iostream>
#include <cstring>
#include <vector>


using namespace std;

TEST(TestVulkanInstance, TestInitDestroy) {
  auto& instance = VulkanInstance::Get();

  // Test getters before init()
  EXPECT_EQ(instance.getVkInstance(), VK_NULL_HANDLE);
  EXPECT_EQ(instance.getDevices().size(), 0);
  EXPECT_EQ(instance.getDevices().size(), 0);

  // Call init() and test getters
  ASSERT_TRUE(instance.init());
  EXPECT_NE(instance.getVkInstance(), VK_NULL_HANDLE);
  EXPECT_GT(instance.getDevices().size(), 0);

  // Call destroy() and test getters
  instance.destroy();
  EXPECT_EQ(instance.getVkInstance(), VK_NULL_HANDLE);
  EXPECT_EQ(instance.getDevices().size(), 0);
}

TEST(TestVulkanInstance, TestLayers) {
  auto& instance = VulkanInstance::Get();

  // Should reject an unknown layer
  EXPECT_FALSE(instance.addValidationLayer("Non-Layer"));

  const auto& layers = instance.getAvailableLayers();
  if (layers.size() > 0) {
    // Add the First layer
    EXPECT_TRUE(instance.addValidationLayer(layers.front().layerName));
    // Initalize Instance
    ASSERT_TRUE(instance.init());
    // Should not be able to add layers after initalization
    EXPECT_FALSE(instance.addValidationLayer(layers.back().layerName));
  }
  instance.destroy();
}

TEST(TestVulkanInstance, TestExtensions) {
  auto& instance = VulkanInstance::Get();

  // Should reject an unknown extension
  EXPECT_FALSE(instance.addExtension("Non-Extension"));

  const auto& exentions = instance.getAvailableExentions();
  if (exentions.size() > 0) {
    // Add the First Extension
    EXPECT_TRUE(instance.addExtension(exentions.front().extensionName));
    // Initalize Instance
    ASSERT_TRUE(instance.init());
    // Should not be able to add extensions after initalization
    EXPECT_FALSE(instance.addExtension(exentions.back().extensionName));
  }
  instance.destroy();
}

TEST(TestVulkanInstance, TestDevices) {
  auto& instance = VulkanInstance::Get();

  // Devices should be empty
  const auto& devices = instance.getDevices();
  EXPECT_EQ(devices.size(), 0);
  EXPECT_EQ(instance.pickDevice(), nullptr);

  // Initalize Instance
  ASSERT_TRUE(instance.init());

  // Devices should be populated
  EXPECT_GT(devices.size(), 0);
  EXPECT_NE(instance.pickDevice(), nullptr);

  // Destroy Instance
  instance.destroy();

  // Devices should be de-populated
  EXPECT_EQ(devices.size(), 0);
  EXPECT_EQ(instance.pickDevice(), nullptr);
}
