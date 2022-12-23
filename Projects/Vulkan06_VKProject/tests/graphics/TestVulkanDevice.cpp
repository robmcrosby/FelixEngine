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


TEST_F(TestVulkanDevice, TestDevice) {
  cout << mDevice->name() << endl;
  cout << mDevice->type() << endl;
  ASSERT_TRUE(true);
}
