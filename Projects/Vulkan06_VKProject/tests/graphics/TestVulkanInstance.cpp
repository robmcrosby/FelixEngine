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

  const auto& layers = instance.getAvailableLayers();
  for (const auto& layer :  layers)
    cout << "Layer: " << layer.layerName << endl;

  EXPECT_TRUE(true);
}

TEST(TestVulkanInstance, TestExtensions) {
  auto& instance = VulkanInstance::Get();

  const auto& exentions = instance.getAvailableExentions();
  for (const auto& extension :  exentions)
    cout << "Extension: " << extension.extensionName << endl;

  EXPECT_TRUE(true);
}
