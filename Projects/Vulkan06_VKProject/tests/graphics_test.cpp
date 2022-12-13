#include <gtest/gtest.h>
#include <VulkanIncludes.hpp>
#include <iostream>


using namespace std;

class VulkanTest : public ::testing::Test {
public:
  VulkanDevicePtr mDevice;
  VulkanQueuePtr mQueue;

protected:
   VulkanTest() {
     //cout << "VulkanTest()" << endl;
     auto& instance = VulkanInstance::Get();
     instance.setApplicationName("Graphics Unit Tests");
     instance.setEngineName("No Engine");
     instance.enableValidation();
     assert(instance.init());
   }

   ~VulkanTest() {
     //cout << "~VulkanTest()" << endl;
     VulkanInstance::Get().destroy();
   }

   void SetUp() override {
     //cout << "SetUp()" << endl;
     mDevice = VulkanInstance::Get().pickDevice();
     assert(mDevice != nullptr);

     mQueue = mDevice->createQueue(VK_QUEUE_COMPUTE_BIT);
     assert(mQueue != nullptr);

     assert(mDevice->init());
  }

  void TearDown() override {
    //cout << "TearDown()" << endl;
    mQueue = nullptr;
    mDevice = nullptr;
  }
};


TEST_F(VulkanTest, TestCreateBuffer) {
  auto buffer = mDevice->createBuffer();
  ASSERT_TRUE(buffer != nullptr);

  buffer->setUsage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
  buffer->setCreateFlags(VMA_ALLOCATION_CREATE_MAPPED_BIT |
    VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
  ASSERT_TRUE(buffer->alloc(64));
}

TEST_F(VulkanTest, TestCreateLayout) {
  auto layout = mDevice->createLayout();
  ASSERT_TRUE(layout != nullptr);
}

TEST_F(VulkanTest, TestCreatePipeline) {
  auto pipeline = mDevice->createPipeline();
  ASSERT_TRUE(pipeline != nullptr);
}

TEST_F(VulkanTest, TestCreateCommand) {
  auto command = mQueue->createCommand();
  ASSERT_TRUE(command != nullptr);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
