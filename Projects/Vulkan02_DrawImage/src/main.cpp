#include "VulkanIncludes.hpp"
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>


using namespace std;

void runDrawImageTest(VulkanDevicePtr device, VulkanQueuePtr queue) {
  int width = 512;
  int height = 512;
  int channels = 4;

  auto outBuffer = device->createBuffer();
  outBuffer->setUsage(VK_BUFFER_USAGE_TRANSFER_DST_BIT |
    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
  outBuffer->setCreateFlags(VMA_ALLOCATION_CREATE_MAPPED_BIT |
    VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);

  auto outImage = device->createImage();

  // Allocate the Vulkan Buffers
  VkDeviceSize size = width * height * channels;
  if (outBuffer->alloc(size)) {
    auto layout = device->createSetLayout();
    layout->setBuffer(outBuffer, 0);
    layout->update();

    auto pipeline = device->createPipeline();
    pipeline->setKernal("clear.spv");

    auto command = queue->createCommand();
    command->begin();
    command->bind(pipeline, layout);
    command->dispatch(width * height);
    command->end();

    queue->submitCommand(command);
    queue->waitIdle();

    // Read from the device
    stbi_write_png("result.png", width, height, channels, outBuffer->data(), width * channels);
    cout << "It Works!" << endl;
  }
  else {
    cerr << "Error: initalizing buffers" << endl;
  }
}

int main() {
  auto& instance = VulkanInstance::Get();
  instance.setApplicationName("VK Draw Image");
  instance.setEngineName("No Engine");
  instance.enableValidation();
  if (instance.init()) {
    auto device = instance.pickDevice();
    if (device) {
      auto queue = device->createQueue(VK_QUEUE_COMPUTE_BIT);
      if (device->init()) {
        runDrawImageTest(device, queue);
      }
    }

    instance.destroy();
  }
  return 0;

  // int width = 512;
  // int height = 512;
  // int channels = 4;
  // vector<uint8_t> img(width * height * channels, 0xff);
  //
  //
  //
  //
  // stbi_write_png("result.png", width, height, channels, img.data(), width * channels);
  // stbi_write_jpg("result.jpg", width, height, channels, img.data(), 100);
  //
  // cout << "It Works!" << endl;
  // return 0;
}
