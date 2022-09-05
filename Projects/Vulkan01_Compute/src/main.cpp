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
        auto memory = device->allocate(1024,
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        //auto buffer = device->createBuffer();

        cout << "It Works!" << endl;
      }
    }

    instance.destroy();
  }
  return 0;
}
