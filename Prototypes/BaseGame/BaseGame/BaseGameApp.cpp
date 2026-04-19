//
//  BaseGameApp.cpp
//  BaseGame
//
//  Created by Robert Crosby on 4/9/26.
//

#include "BaseGameApp.hpp"
#include "CompositeCamera.hpp"
#include "GameCamera.hpp"
#include "TurnTable.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <FelixEngine/SdlEventHandler.hpp>
#include <FelixEngine/GuiEventHandler.hpp>


using namespace std;
using namespace glm;
using namespace Felix;

class TestBinding {
public:
  void onCreate(Entity& entity) {
    cout << "onCreate(" << entity.tag() << ")" << endl;
    entity.get<SdlEventHandler>().connect(entity, this);
    entity.get<GuiEventHandler>().connect(entity, this);
  }
  void onDestory(Entity& entity) {
    cout << "onDestory(" << entity.tag() << ")" << endl;
    entity.get<SdlEventHandler>().disconnect();
    entity.get<GuiEventHandler>().disconnect();
  }
  void onUpdate(Entity& entity, float ts) {
    //cout << "onUpdate(" << entity.tag() << ", " << ts << ")" << endl;
  }
  void onSdlEvent(Entity& entity, SDL_Event* event) {
    if (event->type == SDL_EVENT_FINGER_DOWN)
      cout << entity.tag() << " handled sdl finger down" << endl;
  }
  void onGuiEvent(Entity& entity, const GuiEvent& event) {
    if (event.type == ButtonPressEvent)
      cout << entity.tag() << " handled button press from " << event.sender.tag() << endl;
  }
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
 
  auto mainCamera = mGpuContext->getMainCamera(mScene);
  auto& mainPass = mainCamera.get<GpuPass>();
  mainPass.renderPass->getFrameBuffer()->setDepthStencilBuffer(VK_FORMAT_D32_SFLOAT);
  
  auto fboImage = device->createImage();
  fboImage->setFormat(VK_FORMAT_R8G8B8A8_UNORM);
  fboImage->setUsage(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
  
  auto fboCamera = mGpuContext->createCamera(mainCamera, "fboPass");
  auto& fboPass = fboCamera.get<GpuPass>();
  fboPass.renderPass->getFrameBuffer()->addColorAttachment(fboImage);
  fboPass.renderPass->getFrameBuffer()->setDepthStencilBuffer(VK_FORMAT_D32_SFLOAT);
  fboPass.renderPass->setClearColor({0.1f, 0.1f, 0.4f, 1.0f});
  
  
  //auto image = device->createImage();
  //assert(image->load(queue, "image.png"));
  
  auto sampler = device->createSampler();
  sampler->setFilters(VK_FILTER_LINEAR, VK_FILTER_LINEAR);
  
  auto drawItem = mGpuContext->addDrawItem(mainCamera, "DrawItem");
  assert(mGpuContext->setShaders(drawItem, "StaticSprites.spv", "texture.spv"));
  
  auto layout = drawItem.get<GpuDraw>().layoutSet->at(0);
  //layout->setTexture(1, image, sampler);
  layout->setTexture(1, fboImage, sampler);
  
  auto model = mGpuContext->loadModel(fboCamera, "LargeActionFigure.obj");
  model.get<Transform>().model = translate(rotate(scale({1.0f}, glm::vec3(0.8f, 0.8f, 0.8f)), pi<float>(), vec3(0.0f, 0.0f, 1.0f)), vec3(0.0f, -0.5f, 0.0f));
  
  mainCamera.bind<CompositeCamera>();
  fboCamera.bind<GameCamera>();
  model.bind<TurnTable>();
  //drawItem.bind<TestBinding>();
  
  mGuiContext = GuiContext::create();
  mGuiContext->init(*mGpuContext);
  mGuiContext->addGuiDrawToPass(mainCamera);
  mScene.sort<GpuDraw>();
  
  auto menu = mScene.add("MainMenu");
  menu.add<GuiBounds>();
  auto& menuPanel = menu.get<GuiPanel>();
  menuPanel.offset = {0.0f, 0.0f};
  menuPanel.alignX = GuiAlignCenter;
  menuPanel.alignY = GuiAlignCenter;
  //menuPanel.background = true;
  
  auto label = menu.addChild();
  auto& widgetLabel = label.get<GuiWidget>();
  widgetLabel.order = menuPanel.widgetCount++;
  widgetLabel.type = GuiLabel;
  widgetLabel.alignX = GuiAlignCenter;
  widgetLabel.text = "Select an Option";
  
  auto button1 = menu.addChild("button1");
  auto& widgetButton1 = button1.get<GuiWidget>();
  widgetButton1.order = menuPanel.widgetCount++;
  widgetButton1.type = GuiButton;
  widgetButton1.alignX = GuiAlignCenter;
  widgetButton1.padding = vec2(10.0f, 10.0f);
  widgetButton1.rounding = 8.0f;
  widgetButton1.fillColor = {0.0f, 0.5f, 0.0f, 1.0f};
  widgetButton1.hoverColor = {0.0f, 0.5f, 0.0f, 1.0f};
  widgetButton1.pressColor = {0.4f, 0.0f, 0.0f, 1.0f};
  widgetButton1.text = "Start Game";
  
  auto button2 = menu.addChild("button2");
  auto& widgetButton2 = button2.get<GuiWidget>();
  widgetButton2.order = menuPanel.widgetCount++;
  widgetButton2.type = GuiButton;
  widgetButton2.alignX = GuiAlignCenter;
  widgetButton2.padding = vec2(10.0f, 10.0f);
  widgetButton2.rounding = 8.0f;
  widgetButton2.fillColor = {0.0f, 0.0f, 0.5f, 1.0f};
  widgetButton2.hoverColor = {0.0f, 0.0f, 0.5f, 1.0f};
  widgetButton2.pressColor = {0.4f, 0.0f, 0.0f, 1.0f};
  widgetButton2.text = "Options";
  
  
  auto gamePanel = mScene.add("GameMenu");
  gamePanel.add<GuiBounds>();
  auto& buttonPanel = gamePanel.get<GuiPanel>();
  buttonPanel.offset = {-40.0f, -40.0f};
  buttonPanel.alignX = GuiAlignRight;
  buttonPanel.alignY = GuiAlignBottom;
  
  auto button3 = gamePanel.addChild("button3");
  auto& widgetButton3 = button3.get<GuiWidget>();
  widgetButton3.order = buttonPanel.widgetCount++;
  widgetButton3.type = GuiButton;
  widgetButton3.alignX = GuiAlignCenter;
  widgetButton3.padding = vec2(10.0f, 10.0f);
  widgetButton3.rounding = 8.0f;
  widgetButton3.fillColor = {0.0f, 0.5f, 0.0f, 1.0f};
  widgetButton3.hoverColor = {0.0f, 0.5f, 0.0f, 1.0f};
  widgetButton3.pressColor = {0.4f, 0.0f, 0.0f, 1.0f};
  widgetButton3.text = "Pause";
  
  mScene.sort<GuiWidget>();
  mScene.resume();
  return SDL_APP_CONTINUE;
}

SDL_AppResult BaseGameApp::iterate() {
  mScene.updateEvents();
  mScene.onUpdate();
  mGpuContext->draw(mScene);
  return SDL_APP_CONTINUE;
}

SDL_AppResult BaseGameApp::handle(SDL_Event *event) {
  switch (event->type) {
    case SDL_EVENT_QUIT:
      /* App was closed by the user. */
      cout << "SDL Event App Quit" << endl;
      mScene.destory();
      return SDL_APP_SUCCESS;
    case SDL_EVENT_TERMINATING:
      /* Terminate the app.
         Shut everything down before returning from this function.
      */
      cout << "SDL Event App Terminating" << endl;
      mScene.destory();
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
      mScene.pause();
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
      mGpuContext->pause();
      return SDL_APP_CONTINUE;
    case SDL_EVENT_DID_ENTER_FOREGROUND:
      /* Restart your loops here.
         Your app is interactive and getting CPU again.
      */
      cout << "SDL Event App Did Enter Foreground" << endl;
      mGpuContext->resize(mScene);
      mGpuContext->resume(mScene);
      mScene.resume();
      return SDL_APP_CONTINUE;
    case SDL_EVENT_WINDOW_RESIZED:
      /* Window Resized */
      cout << "SDL Event Window Resized" << endl;
      if (!mGpuContext->isPaused()) {
        mGpuContext->resize(mScene);
        mScene.handle(event);
        mGpuContext->resume(mScene);
      }
      return SDL_APP_CONTINUE;
    default:
      /* Handle Other Events */
      if (!mGuiContext || !mGuiContext->handle(event))
        mScene.handle(event);
      return SDL_APP_CONTINUE;
  }
}

void BaseGameApp::quit(SDL_AppResult result) {
  SDL_Quit();
}
