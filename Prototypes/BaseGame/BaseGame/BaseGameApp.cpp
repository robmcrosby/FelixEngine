//
//  BaseGameApp.cpp
//  BaseGame
//
//  Created by Robert Crosby on 4/9/26.
//

#include "BaseGameApp.hpp"
#include <glm/gtc/matrix_transform.hpp>


using namespace std;
using namespace glm;
using namespace Felix;


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



BaseGameApp::BaseGameApp() {
  
}

BaseGameApp::~BaseGameApp() {
  
}

SDL_AppResult BaseGameApp::init() {
  // Initalize SDL
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
    return SDL_APP_FAILURE;
  }
  
  mGpuContext = GpuContext::create();
  if (!mGpuContext->init("BaseGame")) {
    mGpuContext = nullptr;
    return SDL_APP_FAILURE;
  }
  
  auto device = mGpuContext->getDevice();
  auto queue = mGpuContext->getQueue();
 
  auto cameraItem = mGpuContext->getMainCamera(mScene);
  auto& camera = cameraItem.get<Camera>();
  camera.projection = mat4{2.0f};
  
  auto image = device->createImage();
  assert(image->load(queue, "image.png"));
  
  auto sampler = device->createSampler();
  sampler->setFilters(VK_FILTER_LINEAR, VK_FILTER_LINEAR);
  
  auto drawItem = mGpuContext->addDrawItem(cameraItem);
  assert(mGpuContext->setShaders(drawItem, "vertex.spv", "texture.spv"));
  
  auto layout = drawItem.get<GpuDraw>().layoutSet->at(0);
  layout->setTexture(1, image, sampler);
  
  auto mesh = mGpuContext->setMesh(drawItem, rectVerts, rectIndices);
  mesh->addAttribute(0, 0, 2, 0);
  mesh->addAttribute(0, 1, 2, 2);
  
  return SDL_APP_CONTINUE;
}

SDL_AppResult BaseGameApp::iterate() {
  mGpuContext->draw(mScene);
  return SDL_APP_CONTINUE;
}

SDL_AppResult BaseGameApp::handle(SDL_Event *event) {
  return SDL_APP_CONTINUE;
}

void BaseGameApp::quit(SDL_AppResult result) {
  SDL_Quit();
}
