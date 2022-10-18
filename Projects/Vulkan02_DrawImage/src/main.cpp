#include "VulkanIncludes.hpp"
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>


using namespace std;

void testClearBuffer(VulkanDevicePtr device, VulkanQueuePtr queue) {
  int width = 256;
  int height = 256;
  int channels = 4;

  auto outBuffer = device->createBuffer();
  outBuffer->setUsage(
    VK_BUFFER_USAGE_TRANSFER_DST_BIT |
    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
  );
  outBuffer->setCreateFlags(
    VMA_ALLOCATION_CREATE_MAPPED_BIT |
    VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT
  );

  VkDeviceSize size = width * height * channels;
  if (outBuffer->alloc(size)) {
    auto layout = device->createSetLayout();
    layout->setBuffer(outBuffer, 0);
    layout->update();

    auto pipeline = device->createPipeline();
    pipeline->setKernal("clearBuffer.spv");

    if (auto command = queue->beginSingleCommand()) {
      command->bind(pipeline, layout);
      command->dispatch(width * height);
      command->endSingle();
      queue->waitIdle();
    }

    // Read from the device
    stbi_write_png("ResultClearBuffer.png", width, height, channels, outBuffer->data(), width * channels);
    cout << "It Works!" << endl;
  }
  else {
    cerr << "Error: initalizing buffer" << endl;
  }
}

void testClearTexture(VulkanDevicePtr device, VulkanQueuePtr queue) {
  int width = 512;
  int height = 512;
  int channels = 4;

  auto outBuffer = device->createBuffer();
  outBuffer->setUsage(
    VK_BUFFER_USAGE_TRANSFER_DST_BIT |
    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
  );
  outBuffer->setCreateFlags(
    VMA_ALLOCATION_CREATE_MAPPED_BIT |
    VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT
  );

  auto outImage = device->createImage();
  outImage->setUsage(
    VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
    VK_IMAGE_USAGE_STORAGE_BIT
  );

  // Allocate the Vulkan Buffers
  VkDeviceSize size = width * height * channels;
  if (outBuffer->alloc(size) && outImage->alloc(width, height)) {
    queue->transition(
      outImage,
      VK_IMAGE_LAYOUT_GENERAL,
      VK_ACCESS_TRANSFER_WRITE_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT
    );

    auto layout = device->createSetLayout();
    layout->setTexture(outImage, 0);
    layout->update();

    auto pipeline = device->createPipeline();
    pipeline->setKernal("clearTexture.spv");

    if (auto command = queue->beginSingleCommand()) {
      command->bind(pipeline, layout);
      command->dispatch(width, height);
      command->endSingle();
      queue->waitIdle();
    }

    queue->transition(
      outImage,
      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
      VK_ACCESS_TRANSFER_READ_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT
    );

    // Read from the device
    queue->copyImageToBuffer(outImage, outBuffer);
    stbi_write_png("ResultClearTexture.png", width, height, channels, outBuffer->data(), width * channels);
    cout << "It Works!" << endl;
  }
  else {
    cerr << "Error: initalizing buffers" << endl;
  }
}

void testDesaturate(VulkanDevicePtr device, VulkanQueuePtr queue) {
  int width, height, channels;

  stbi_uc* pixels = stbi_load("image.png", &width, &height, &channels, STBI_rgb_alpha);
  if (!pixels) {
    cerr << "Error reading image file" << endl;
    return;
  }
  //cout << "load image: (" << width << ", " << height << ") " << channels << endl;

  auto buffer = device->createBuffer();
  buffer->setUsage(
    VK_BUFFER_USAGE_TRANSFER_DST_BIT |
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
  );
  buffer->setCreateFlags(
    VMA_ALLOCATION_CREATE_MAPPED_BIT |
    VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT
  );

  auto inImage = device->createImage();
  inImage->setUsage(
    VK_IMAGE_USAGE_TRANSFER_DST_BIT |
    VK_IMAGE_USAGE_STORAGE_BIT
  );

  auto outImage = device->createImage();
  outImage->setUsage(
    VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
    VK_IMAGE_USAGE_STORAGE_BIT
  );

  // Allocate the Vulkan Buffers
  VkDeviceSize size = width * height * channels;
  if (buffer->alloc(size) && inImage->alloc(width, height) && outImage->alloc(width, height)) {
    memcpy(buffer->data(), pixels, size);
    queue->transition(
      inImage,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      VK_ACCESS_TRANSFER_WRITE_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT
    );
    queue->copyBufferToImage(buffer, inImage);
    queue->transition(
      inImage,
      VK_IMAGE_LAYOUT_GENERAL,
      VK_ACCESS_TRANSFER_READ_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT
    );

    queue->transition(
      outImage,
      VK_IMAGE_LAYOUT_GENERAL,
      VK_ACCESS_TRANSFER_WRITE_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT
    );

    auto layout = device->createSetLayout();
    layout->setTexture(inImage, 0);
    layout->setTexture(outImage, 1);
    layout->update();

    auto pipeline = device->createPipeline();
    pipeline->setKernal("desaturate.spv");

    if (auto command = queue->beginSingleCommand()) {
      command->bind(pipeline, layout);
      command->dispatch(width, height);
      command->endSingle();
      queue->waitIdle();
    }

    queue->transition(
      outImage,
      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
      VK_ACCESS_TRANSFER_READ_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT
    );

    // Read from the device
    queue->copyImageToBuffer(outImage, buffer);
    stbi_write_png("ResultDesaturate.png", width, height, channels, buffer->data(), width * channels);
    //stbi_write_png("clearTextureResult.png", width, height, channels, outImage->data(), width * channels);
    cout << "It Works!" << endl;
  }
  else {
    cerr << "Error: initalizing buffers" << endl;
  }

  stbi_image_free(pixels);
}

int main() {
  auto& instance = VulkanInstance::Get();
  instance.setApplicationName("VK Draw Image");
  instance.setEngineName("No Engine");
  instance.enableValidation();
  if (instance.init()) {
    if (auto device = instance.pickDevice()) {
      auto queue = device->createQueue(VK_QUEUE_COMPUTE_BIT);
      if (device->init()) {
        testClearBuffer(device, queue);
        testClearTexture(device, queue);
        testDesaturate(device, queue);
      }
    }
    instance.destroy();
  }
  return 0;
}
