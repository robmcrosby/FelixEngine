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
const vector<uint32_t> rectIndices = {
  1, 0, 3, 2, 0, 3
};

void testClearBuffer(VulkanDevicePtr device, VulkanQueuePtr queue, VulkanBufferPtr buffer) {
  auto layoutSet = device->createLayoutSet();
  auto layout = layoutSet->at(0);
  layout->setBuffer(buffer, 0);
  layout->update();

  auto pipeline = device->createPipeline();
  pipeline->setKernalShader("clearBuffer.spv");

  if (auto command = queue->beginSingleCommand()) {
    command->bind(pipeline, layoutSet);
    command->dispatch(buffer->size() / (4 * 256));
    command->endSingle();
    queue->waitIdle();
  }
}

void testDrawTriangle(VulkanDevicePtr device, VulkanQueuePtr queue, VulkanImagePtr image) {
  auto framebuffer = device->createFrameBuffer();
  framebuffer->addColorAttachment(image);

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
}

void testDrawRectangleStrip(VulkanDevicePtr device, VulkanQueuePtr queue, VulkanImagePtr image) {
  auto framebuffer = device->createFrameBuffer();
  framebuffer->addColorAttachment(image);

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
    command->draw(mesh);
    command->endRenderPass();
    command->endSingle();
    queue->waitIdle();
  }
}

void testDrawRectangleIndexed(VulkanDevicePtr device, VulkanQueuePtr queue, VulkanImagePtr image) {
  auto framebuffer = device->createFrameBuffer();
  framebuffer->addColorAttachment(image);

  auto renderPass = device->createRenderPass();
  renderPass->setFramebuffer(framebuffer);

  auto mesh = device->createMesh();
  mesh->addBuffer(queue, rectVerts, 5);
  mesh->addAttribute(0, 0, 2, 0);
  mesh->addAttribute(0, 1, 3, 2);
  mesh->setIndexBuffer(queue, rectIndices);

  auto pipeline = device->createPipeline();
  pipeline->setVertexShader("vertexColor.spv");
  pipeline->setFragmentShader("color.spv");

  if (auto command = queue->beginSingleCommand()) {
    command->beginRenderPass(renderPass);
    command->bind(pipeline, renderPass, mesh);
    command->draw(mesh);
    command->endRenderPass();
    command->endSingle();
    queue->waitIdle();
  }
}

void testDrawRectangleInstanced(VulkanDevicePtr device, VulkanQueuePtr queue, VulkanImagePtr image) {
  auto framebuffer = device->createFrameBuffer();
  framebuffer->addColorAttachment(image);

  auto renderPass = device->createRenderPass();
  renderPass->setFramebuffer(framebuffer);

  auto mesh = device->createMesh();
  mesh->addBuffer(queue, rectVerts, 5);
  mesh->addAttribute(0, 0, 2, 0);
  mesh->addAttribute(0, 1, 3, 2);
  mesh->setIndexBuffer(queue, rectIndices);

  auto pipeline = device->createPipeline();
  pipeline->setVertexShader("vertexInstanced.spv");
  pipeline->setFragmentShader("color.spv");

  if (auto command = queue->beginSingleCommand()) {
    command->beginRenderPass(renderPass);
    command->bind(pipeline, renderPass, mesh);
    command->draw(mesh, 4);
    command->endRenderPass();
    command->endSingle();
    queue->waitIdle();
  }
}

void transitionImageToRender(VulkanQueuePtr queue, VulkanImagePtr image) {
  queue->transition(
    image,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
  );
}

void copyImageToBuffer(VulkanQueuePtr queue, VulkanImagePtr image, VulkanBufferPtr buffer) {
  queue->transition(
    image,
    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
    VK_ACCESS_TRANSFER_READ_BIT,
    VK_PIPELINE_STAGE_TRANSFER_BIT
  );
  queue->copyImageToBuffer(image, buffer);
}

void runDrawTests(VulkanDevicePtr device, VulkanQueuePtr queue) {
  int width = 512;
  int height = 512;
  int channels = 4;

  auto image = device->createImage();
  image->setUsage(
    VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
  );

  auto buffer = device->createBuffer();
  buffer->setUsage(
    VK_BUFFER_USAGE_TRANSFER_DST_BIT |
    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
  );
  buffer->setCreateFlags(
    VMA_ALLOCATION_CREATE_MAPPED_BIT |
    VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT
  );

  if (image->alloc(width, height) && buffer->alloc(image->size())) {
    testClearBuffer(device, queue, buffer);
    stbi_write_png("ResultClearBuffer.png", width, height, channels, buffer->data(), width * channels);

    transitionImageToRender(queue, image);
    testDrawTriangle(device, queue, image);
    copyImageToBuffer(queue, image, buffer);
    stbi_write_png("ResultTriangle.png", width, height, channels, buffer->data(), width * channels);

    transitionImageToRender(queue, image);
    testDrawRectangleStrip(device, queue, image);
    copyImageToBuffer(queue, image, buffer);
    stbi_write_png("ResultRectangleStrip.png", width, height, channels, buffer->data(), width * channels);

    transitionImageToRender(queue, image);
    testDrawRectangleIndexed(device, queue, image);
    copyImageToBuffer(queue, image, buffer);
    stbi_write_png("ResultRectangleIndexed.png", width, height, channels, buffer->data(), width * channels);

    transitionImageToRender(queue, image);
    testDrawRectangleInstanced(device, queue, image);
    copyImageToBuffer(queue, image, buffer);
    stbi_write_png("ResultRectangleInstanced.png", width, height, channels, buffer->data(), width * channels);
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
        runDrawTests(device, queue);
      }
    }
    instance.destroy();
  }
  return 0;
}
