#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include "VulkanIncludes.hpp"
#include "Matrix.hpp"
#include <vulkan/vulkan.h>

using namespace std;

struct Vertex {
  vec2 position;
  vec2 uv;
};

const vector<Vertex> rectVerts = {
  {{-0.5f, -0.5f},  {0.0f, 0.0f}},
  {{-0.5f,  0.5f},  {0.0f, 1.0f}},
  {{ 0.5f, -0.5f},  {1.0f, 0.0f}},
  {{ 0.5f,  0.5f},  {1.0f, 1.0f}}
};
const vector<uint32_t> rectIndices = {
  1, 0, 3, 2, 0, 3
};

void runLoop(SDL_Window* window, VulkanDevicePtr device, VulkanQueuePtr queue) {
  bool quit = false;

  auto swapChain = device->createSwapChain();
  swapChain->setToWindow(window);

  auto framebuffer = device->createFrameBuffer();
  framebuffer->addColorAttachment(swapChain->getPresentImage());

  auto frameSync = device->createFrameSync();
  frameSync->setup(swapChain->frames(), 2);

  auto image = device->createImage();
  image->load(queue, "image.png");

  auto sampler = device->createSampler();
  sampler->setFilters(VK_FILTER_LINEAR, VK_FILTER_LINEAR);

  auto layoutSet = device->createLayoutSet(swapChain->frames());
  auto layout = layoutSet->at(0);
  mat4 modelMatrix;
  layout->setUniform(0, modelMatrix);
  layout->setTexture(1, image, sampler);
  layout->update();

  auto mesh = device->createMesh();
  mesh->addBuffer(queue, rectVerts);
  mesh->addAttribute(0, 0, 2, 0);
  mesh->addAttribute(0, 1, 2, 2);
  mesh->setIndexBuffer(queue, rectIndices);

  auto renderPass = device->createRenderPass();
  renderPass->setFramebuffer(framebuffer);

  auto pipeline = device->createPipeline();
  pipeline->setVertexShader("vertexUV.spv");
  pipeline->setFragmentShader("texture.spv");

  auto command = queue->createCommand(swapChain->frames());

  VulkanRecording recording = [&](int frame) {
    command->begin(frame);
    command->beginRenderPass(renderPass);
    command->bind(pipeline, renderPass, mesh, layoutSet);
    command->draw(mesh);
    command->endRenderPass();
    command->end();
  };

  for (int frame = 0; frame < swapChain->frames(); ++frame)
    recording(frame);

  float scale = 1.0;
  float offsetX = 0.0;
  float offsetY = 0.0;
  SDL_Event event;
  while (!quit) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        quit = true;
      }
      else if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
          //cout << "Size Change" << endl;
          queue->waitIdle();
          swapChain->rebuild();
          renderPass->rebuild();
          pipeline->rebuild();

          for (int frame = 0; frame < swapChain->frames(); ++frame)
            recording(frame);
        }
      }
      else if (event.type == SDL_KEYDOWN) {
        switch(event.key.keysym.sym) {
          case SDLK_LEFT:
            offsetX -= 0.1;
            break;
          case SDLK_RIGHT:
            offsetX += 0.1;
            break;
          case SDLK_UP:
            offsetY -= 0.1;
            break;
          case SDLK_DOWN:
            offsetY += 0.1;
            break;
          case SDLK_SPACE:
            scale += 0.1;
            break;
          case SDLK_LSHIFT:
          case SDLK_RSHIFT:
            scale -= 0.1;
            break;
          default:
            break;
        }
      }
    }

    // Get Next Frame from Swap Chain
    int frame = swapChain->getNextFrame(frameSync);

    if (frame >= 0) {
      // Update Matrix
      modelMatrix.x.x = scale;
      modelMatrix.y.y = scale;
      modelMatrix.w.x = offsetX;
      modelMatrix.w.y = offsetY;
      layout->update(0, modelMatrix, frame);

      // Update Command Buffer Recording
      //recording(frame);

      // Submit Command Buffer and Present Frame
      queue->submitCommand(command, frameSync);
      swapChain->presentFrame(frameSync, queue);
    }

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
    "Resize SDL",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    512, 512,
    SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
  );

  CStrings sdlExtensions;
  getSDL2Extenstions(window, sdlExtensions);

  auto& instance = VulkanInstance::Get();
  instance.setApplicationName("VK Hello SDL");
  instance.setEngineName("No Engine");
  instance.enableValidation();
  for (string extension : sdlExtensions)
    instance.addExtension(extension);

  if (instance.init()) {
    if (auto device = instance.pickDevice()) {
      device->addExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
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
