#include "VulkanIncludes.hpp"


using namespace std;

void runComputeTest(VulkanDevicePtr device, VulkanQueuePtr queue) {
  auto buffer = device->createBuffer();
  buffer->setUsage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
  buffer->setCreateFlags(VMA_ALLOCATION_CREATE_MAPPED_BIT |
    VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);
  if (buffer->alloc(65536)) {
    cout << "It Works!" << endl;
  }
  else {
    cerr << "Error: initalizing buffer" << endl;
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
