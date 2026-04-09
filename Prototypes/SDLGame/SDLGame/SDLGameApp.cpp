//
//  SDLGameApp.cpp
//  SDLGame
//
//  Created by Robert Crosby on 3/19/26.
//

#include "SDLGameApp.hpp"

#include "GpuContext.hpp"
#include "GuiContext.hpp"
#include "EcsEntity.hpp"
#include "EcsEvent.hpp"

#include <glm/gtc/matrix_transform.hpp>


using namespace std;
using namespace glm;


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


struct RotateOnZ {
  bool rotate = false;
  EcsEntity mEntity;
  
  void onCreate(EcsEntity& entity) {
    mEntity = entity;
    entity.get<ListenerComp>().connect(entity, *this);
  }
  void onDestory(EcsEntity& entity) {}
  void onUpdate(EcsEntity& entity, float ts) {
    if (!rotate)
      return;
    float angle = glm::radians(50.0f * ts);
    auto& transform = entity.get<TransformComp>();
    transform.transform.model = glm::rotate(transform.transform.model, angle, vec3(0.0f, 0.0f, 1.0f));
  }
  void onEvent(const EcsEvent& event) {
    if (event == ButtonPressEvent) {
      string tag = event.sender.get<TagComp>().tag;
      if (tag == "Button1") {
        rotate = !rotate;
      } else if (tag == "Properties") {
        mEntity.disconnect();
      }
    }
  }
};



SDLGameApp::SDLGameApp(): mGpuContext(nullptr), mLastTime(0) {
  
}

SDLGameApp::~SDLGameApp() {
  
}

SDL_AppResult SDLGameApp::init() {
  // Initalize SDL
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
    return SDL_APP_FAILURE;
  }
  
  mGpuContext = GpuContext::create();
  if (!mGpuContext->init()) {
    mGpuContext = nullptr;
    return SDL_APP_FAILURE;
  }
  
  auto device = mGpuContext->getDevice();
  auto queue = mGpuContext->getQueue();
  
  auto cameraItem = mGpuContext->getMainCamera(mScene);
  auto& cameraComp = cameraItem.get<CameraComp>();
  cameraComp.camera.projection = mat4{2.0f};
  
  auto image = device->createImage();
  assert(image->load(queue, "image.png"));
  
  auto sampler = device->createSampler();
  sampler->setFilters(VK_FILTER_LINEAR, VK_FILTER_LINEAR);
  
  auto drawItem = mGpuContext->addDrawItem(cameraItem);
  assert(mGpuContext->setShaders(drawItem, "vertex.spv", "texture.spv"));
  
  auto layout = drawItem.get<GpuDrawComp>().layoutSet->at(0);
  layout->setTexture(1, image, sampler);
  
  auto mesh = mGpuContext->setMesh(drawItem, rectVerts, rectIndices);
  mesh->addAttribute(0, 0, 2, 0);
  mesh->addAttribute(0, 1, 2, 2);
  
  drawItem.add<ScriptComp>().bind<RotateOnZ>();
  
  // Create GUI Context
  mGuiContext = GuiContext::create();
  mGuiContext->init(*mGpuContext);
  
  auto guiItem = mScene.create();
  auto& guiDraw = guiItem.get<GpuDrawComp>();
  guiDraw.guiContext = mGuiContext;
  guiDraw.order = INT_MAX;
  guiDraw.pass = cameraItem.handle();
  
  auto menu = mScene.createGuiPanel("Menu");
  auto& menuPanel = menu.get<GuiPanelComp>();
  menuPanel.offset = {0.0f, 0.0f};
  menuPanel.alignX = GuiAlignCenter;
  menuPanel.alignY = GuiAlignCenter;
  menu.addGuiLabel("Select an Option");
  menu.addGuiButton("Button1").get<GuiWidgetComp>().text = "Start Game";
  menu.addGuiButton("Properties");
  
  auto menu2 = mScene.createGuiPanel("Menu2");
  auto& menu2Panel = menu2.get<GuiPanelComp>();
  menu2Panel.offset = {-20.0f,-60.0f};
  menu2Panel.alignX = GuiAlignRight;
  menu2Panel.alignY = GuiAlignBottom;
  menu2.addGuiLabel("Alt Options");
  menu2.addGuiButton("Button2").get<GuiWidgetComp>().text = "Start Game";
  
  mScene.sortGui();
  mScene.onWindowResize(mGpuContext->getWindowSize());
  
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDLGameApp::iterate() {
  // Get times
  uint64_t currentTime = SDL_GetTicks();
  uint64_t elapsedTime = currentTime - mLastTime;
  mLastTime = currentTime;
  
  // Update Scene
  mScene.updateEvents();
  mScene.onUpdate(currentTime, elapsedTime);
  
  // Draw Scene
  mGpuContext->draw(mScene);
  
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDLGameApp::handle(SDL_Event *event) {
  switch (event->type) {
    case SDL_EVENT_QUIT:
      /* App was closed by the user. */
      cout << "SDL Event App Quit" << endl;
      return SDL_APP_SUCCESS;
    case SDL_EVENT_TERMINATING:
      /* Terminate the app.
         Shut everything down before returning from this function.
      */
      cout << "SDL Event App Terminating" << endl;
      return SDL_APP_CONTINUE;
    case SDL_EVENT_LOW_MEMORY:
      /* You will get this when your app is paused and iOS wants more memory.
         Release as much memory as possible.
      */
      cout << "SDL Event Low Memory Warining" << endl;
      return SDL_APP_CONTINUE;
    case SDL_EVENT_WILL_ENTER_BACKGROUND:
      /* Prepare your app to go into the background.  Stop loops, etc.
         This gets called when the user hits the home button, or gets a call.
      */
      cout << "SDL Event App Will Enter Background" << endl;
      return SDL_APP_CONTINUE;
    case SDL_EVENT_DID_ENTER_BACKGROUND:
      /* This will get called if the user accepted whatever sent your app to the background.
         If the user got a phone call and canceled it, you'll instead get an SDL_EVENT_DID_ENTER_FOREGROUND event and restart your loops.
         When you get this, you have 5 seconds to save all your state or the app will be terminated.
         Your app is NOT active at this point.
      */
      cout << "SDL Event App Did Enter Background" << endl;
      mGpuContext->pause();
      return SDL_APP_CONTINUE;
    case SDL_EVENT_WILL_ENTER_FOREGROUND:
      /* This call happens when your app is coming back to the foreground.
         Restore all your state here.
      */
      cout << "SDL Event App Will Enter Foreground" << endl;
      return SDL_APP_CONTINUE;
    case SDL_EVENT_DID_ENTER_FOREGROUND:
      /* Restart your loops here.
         Your app is interactive and getting CPU again.
      */
      cout << "SDL Event App Did Enter Foreground" << endl;
      mGpuContext->resume(mScene);
      return SDL_APP_CONTINUE;
    case SDL_EVENT_WINDOW_RESIZED:
      /* Window Resized */
      cout << "SDL Event Window Resized" << endl;
      mGpuContext->resize(mScene);
      return SDL_APP_CONTINUE;
    default:
      /* Handle Other Events */
      mGuiContext->handle(event);
      return SDL_APP_CONTINUE;
  }
}

void SDLGameApp::quit(SDL_AppResult result) {
  mGuiContext = nullptr;
  mGpuContext = nullptr;
  SDL_Quit();
}
