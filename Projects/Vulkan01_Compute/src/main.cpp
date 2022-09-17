#include "VulkanIncludes.hpp"
#include <array>


using namespace std;

void runComputeTest(VulkanDevicePtr device, VulkanQueuePtr queue) {
  // Create an Array of random floats
  array<float, 32> bufferSrc, bufferResult;
  for (auto& f : bufferSrc)
    f = (rand()/(RAND_MAX/10000))/100.0f;

  // Create A Vulkan Buffer for writing [CPU -> GPU]
  auto bufferIn = device->createBuffer();
  bufferIn->setUsage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
  bufferIn->setCreateFlags(VMA_ALLOCATION_CREATE_MAPPED_BIT |
    VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT );

  // Cerate A Vulkan Buffer to read from [GPU -> CPU]
  auto bufferOut = device->createBuffer();
  bufferOut->setUsage(VK_BUFFER_USAGE_TRANSFER_DST_BIT);
  bufferOut->setCreateFlags(VMA_ALLOCATION_CREATE_MAPPED_BIT |
    VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);

  // Allocate the Vulkan Buffers
  VkDeviceSize size = bufferSrc.size() * sizeof(bufferSrc[0]);
  if (bufferIn->alloc(size) && bufferOut->alloc(size)) {
    // Wirte to the device
    memcpy(bufferIn->data(), bufferSrc.data(), size);

    auto command = queue->createCommand();

    cout << "It Works!" << endl;

    // Read from the device
    memcpy(bufferResult.data(), bufferOut->data(), size);
    for (int i = 0; i < bufferResult.size(); ++i)
      cout << "  " << bufferSrc[i] << "\t-> " << bufferResult[i] << endl;
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
