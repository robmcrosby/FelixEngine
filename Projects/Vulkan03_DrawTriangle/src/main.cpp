#include "VulkanIncludes.hpp"
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>


using namespace std;

void testDrawTriangle(VulkanDevicePtr device, VulkanQueuePtr queue) {
  cout << "It Works!" << endl;
}

int main() {
  auto& instance = VulkanInstance::Get();
  instance.setApplicationName("VK Draw Triangle");
  instance.setEngineName("No Engine");
  instance.enableValidation();
  if (instance.init()) {
    if (auto device = instance.pickDevice()) {
      auto queue = device->createQueue(VK_QUEUE_GRAPHICS_BIT);
      if (device->init()) {
        testDrawTriangle(device, queue);
      }
    }
    instance.destroy();
  }
  return 0;
}
