#include <gtest/gtest.h>
#include <VulkanIncludes.hpp>
#include <iostream>
#include <cstring>
#include <vector>

#define ARRAY_SIZE 64

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

   static void fillRandFloats(vector<float>& v) {
     for (auto& f : v)
       f = (rand()/(RAND_MAX/10000))/100.0f;
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

TEST_F(VulkanTest, TestCompute) {
  vector<float> src(ARRAY_SIZE, 0), result(ARRAY_SIZE, 0);
  fillRandFloats(src);

  // Buffer for writing to device [CPU -> GPU]
  auto inBuffer = mDevice->createBuffer();
  inBuffer->setUsage(VK_BUFFER_USAGE_TRANSFER_DST_BIT |
    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
  inBuffer->setCreateFlags(VMA_ALLOCATION_CREATE_MAPPED_BIT |
    VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT );

  // Buffer for reading from device [GPU -> CPU]
  auto outBuffer = mDevice->createBuffer();
  outBuffer->setUsage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
  outBuffer->setCreateFlags(VMA_ALLOCATION_CREATE_MAPPED_BIT |
    VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);

  // Allocate the Vulkan Buffers and write to device
  VkDeviceSize size = src.size() * sizeof(src[0]);
  ASSERT_TRUE(inBuffer->alloc(size) && outBuffer->alloc(size));
  memcpy(inBuffer->data(), src.data(), size);

  // Assign the buffers to a Layout Set
  auto layoutSet = mDevice->createLayoutSet();
  auto layout = layoutSet->at(0);
  layout->setBuffer(0, inBuffer);
  layout->setBuffer(1, outBuffer);
  layout->update();

  // Create a compute pipeline
  auto pipeline = mDevice->createPipeline();
  ASSERT_TRUE(pipeline->setKernalShader("shaders/copy.spv"));

  // Record the compute command buffer
  auto command = mQueue->createCommand();
  command->begin();
  command->bind(pipeline, layoutSet);
  command->dispatch(1);
  command->end();

  // Execude the compute command buffer
  mQueue->submitCommand(command);
  mQueue->waitIdle();

  // Read from the device and check results
  memcpy(result.data(), outBuffer->data(), size);
  for (int i = 0; i < src.size(); ++i)
    EXPECT_EQ(result[i], src[i]);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
