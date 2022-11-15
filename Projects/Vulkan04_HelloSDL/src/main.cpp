#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include "VulkanIncludes.hpp"
#include <vulkan/vulkan.h>


using namespace std;


void runLoop(SDL_Window* window, VulkanDevicePtr device, VulkanQueuePtr queue) {
  bool quit = false;

  auto swapChain = device->createSwapChain();
  swapChain->setToWindow(window);

  auto framebuffer = device->createFrameBuffer();
  framebuffer->addColorAttachment(swapChain->getPresentImage());

  auto frameSync = swapChain->getFrameSync();

  auto renderPass = device->createRenderPass();
  renderPass->setFramebuffer(framebuffer);

  auto pipeline = device->createPipeline();
  pipeline->setVertexShader("triangle.spv");
  pipeline->setFragmentShader("color.spv");

  auto command = queue->createCommand(swapChain->frames());
  for (int frame = 0; frame < swapChain->frames(); ++frame) {
    command->begin(frame);
    command->beginRenderPass(renderPass);
    command->bind(pipeline, renderPass);
    command->draw(3);
    command->endRenderPass();
    command->end();
  }

  SDL_Event e;
  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }

    int frame = swapChain->getNextFrame();
    queue->submitCommand(command, frameSync);
    swapChain->presentFrame(queue);

    SDL_Delay(10);
  }
  queue->waitIdle();
}

void getSDL2Extenstions(SDL_Window* window, CStrings& extensions) {
  unsigned int count;
  SDL_Vulkan_GetInstanceExtensions(window, &count, NULL);
  extensions.resize(count);
  SDL_Vulkan_GetInstanceExtensions(window, &count, extensions.data());
}

int main(int argc, char *argv[])
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    cerr << "Error Initalizing SDL" << endl;
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow(
    "Hello SDL",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    640, 480,
    SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN
  );

  CStrings sdlExtensions;
  getSDL2Extenstions(window, sdlExtensions);

  auto& instance = VulkanInstance::Get();
  instance.setApplicationName("VK Hello SDL");
  instance.setEngineName("No Engine");
  instance.enableValidation();
  for (string extension : sdlExtensions)
    instance.addExtension(extension);
  instance.addExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  if (instance.init()) {
    if (auto device = instance.pickDevice()) {
      auto queue = device->createQueue(VK_QUEUE_GRAPHICS_BIT);
      if (device->init()) {
        runLoop(window, device, queue);
      }
    }
    instance.destroy();
  }

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
