#include "VulkanIncludes.hpp"


using namespace std;

int main() {
  auto& instance = VulkanInstance::Get();
  instance.setApplicationName("VK Compute");
  instance.setEngineName("No Engine");
  instance.enableValidation();
  if (instance.init()) {
    cout << instance << endl;

    auto device = instance.pickDevice();
    if (device) {
      cout << device << endl;

      auto queue = device->createQueue(VK_QUEUE_COMPUTE_BIT);
      if (device->init()) {
        auto buffer = device->createBuffer();
        cout << "It Works!" << endl;
      }
    }

    instance.destroy();
  }
  return 0;
}
