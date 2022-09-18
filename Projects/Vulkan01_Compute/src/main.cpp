#include "VulkanIncludes.hpp"
#include <array>


using namespace std;

void runComputeTest(VulkanDevicePtr device, VulkanQueuePtr queue) {
  // Create an Array of random floats
  array<float, 32> srcBuffer, resultBuffer;
  for (auto& f : srcBuffer)
    f = (rand()/(RAND_MAX/10000))/100.0f;

  // Create A Vulkan Buffer for writing [CPU -> GPU]
  auto inBuffer = device->createBuffer();
  inBuffer->setUsage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
  inBuffer->setCreateFlags(VMA_ALLOCATION_CREATE_MAPPED_BIT |
    VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT );

  // Cerate A Vulkan Buffer to read from [GPU -> CPU]
  auto outBuffer = device->createBuffer();
  outBuffer->setUsage(VK_BUFFER_USAGE_TRANSFER_DST_BIT);
  outBuffer->setCreateFlags(VMA_ALLOCATION_CREATE_MAPPED_BIT |
    VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);

  // Allocate the Vulkan Buffers
  VkDeviceSize size = srcBuffer.size() * sizeof(srcBuffer[0]);
  if (inBuffer->alloc(size) && outBuffer->alloc(size)) {
    // Wirte to the device
    memcpy(inBuffer->data(), srcBuffer.data(), size);

    auto command = queue->createCommand();
    command->setKernal("copy.spv");

    cout << "It Works!" << endl;

    // Read from the device
    memcpy(resultBuffer.data(), outBuffer->data(), size);
    for (int i = 0; i < resultBuffer.size(); ++i)
      cout << "  " << srcBuffer[i] << "\t-> " << resultBuffer[i] << endl;
  }
  else {
    cerr << "Error: initalizing buffers" << endl;
  }
}

int main() {
  auto& instance = VulkanInstance::Get();
  instance.setApplicationName("VK Compute");
  instance.setEngineName("No Engine");
  instance.enableValidation();
  if (instance.init()) {
    //cout << instance << endl;

    auto device = instance.pickDevice();
    if (device) {
      //cout << device << endl;

      auto queue = device->createQueue(VK_QUEUE_COMPUTE_BIT);
      if (device->init()) {
        runComputeTest(device, queue);
      }
    }

    instance.destroy();
  }
  return 0;
}
