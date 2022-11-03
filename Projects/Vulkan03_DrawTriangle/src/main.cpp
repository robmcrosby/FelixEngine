#include "VulkanIncludes.hpp"
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>


using namespace std;

const vector<float> rectVerts = {
  -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
  -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,
   0.5f, -0.5f,   0.0f, 1.0f, 0.0f,
   0.5f,  0.5f,   0.0f, 0.0f, 1.0f
};

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
    auto layoutSet = device->createLayoutSet();
    auto layout = layoutSet->at(0);
    layout->setBuffer(outBuffer, 0);
    layout->update();

    auto pipeline = device->createPipeline();
    pipeline->setKernalShader("clearBuffer.spv");

    if (auto command = queue->beginSingleCommand()) {
      //command->bind(pipeline, layoutSet);
      command->bind(pipeline);
      command->bind(layoutSet);
      command->dispatch((width * height) / 256);
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

void testDrawTriangle(VulkanDevicePtr device, VulkanQueuePtr queue) {
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
    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
  );

  // Allocate the Vulkan Buffers
  VkDeviceSize size = width * height * channels;
  if (outBuffer->alloc(size) && outImage->alloc(width, height)) {
    queue->transition(
      outImage,
      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    );

    auto framebuffer = device->createFrameBuffer();
    framebuffer->addColorAttachment(outImage);

    auto renderPass = device->createRenderPass();
    renderPass->setFramebuffer(framebuffer);

    auto pipeline = device->createPipeline();
    pipeline->setVertexShader("triangle.spv");
    pipeline->setFragmentShader("color.spv");

    if (auto command = queue->beginSingleCommand()) {
      command->beginRenderPass(renderPass);
      command->bind(pipeline, renderPass);
      command->draw(3);
      command->endRenderPass();
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
    stbi_write_png("ResultTriangle.png", width, height, channels, outBuffer->data(), width * channels);
    cout << "It Works!" << endl;
  }
}

void testDrawRectangle(VulkanDevicePtr device, VulkanQueuePtr queue) {
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
    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
  );

  // Allocate the Vulkan Buffers
  VkDeviceSize size = width * height * channels;
  if (outBuffer->alloc(size) && outImage->alloc(width, height)) {
    queue->transition(
      outImage,
      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    );

    auto framebuffer = device->createFrameBuffer();
    framebuffer->addColorAttachment(outImage);

    auto renderPass = device->createRenderPass();
    renderPass->setFramebuffer(framebuffer);

    auto mesh = device->createMesh();
    mesh->addBuffer(queue, rectVerts, 5);
    mesh->addAttribute(0, 0, 2, 0);
    mesh->addAttribute(0, 1, 3, 2);
    mesh->setTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);

    auto pipeline = device->createPipeline();
    pipeline->setVertexShader("vertexColor.spv");
    pipeline->setFragmentShader("color.spv");

    if (auto command = queue->beginSingleCommand()) {
      command->beginRenderPass(renderPass);
      command->bind(pipeline, renderPass, mesh);
      command->draw(4);
      command->endRenderPass();
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
    stbi_write_png("ResultRectangle.png", width, height, channels, outBuffer->data(), width * channels);
    cout << "It Works!" << endl;
  }
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
        testClearBuffer(device, queue);
        testDrawTriangle(device, queue);
        testDrawRectangle(device, queue);
      }
    }
    instance.destroy();
  }
  return 0;
}
