//
//  BaseGameApp.cpp
//  BaseGame
//
//  Created by Robert Crosby on 4/9/26.
//

#include "BaseGameApp.hpp"
#include <FelixEngine/Tag.hpp>
#include <FelixEngine/UUID.hpp>

using namespace std;
using namespace Felix;

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
  
  auto item = mScene.add("Test");
  cout << "Tag: " << item.get<Tag>().tag << endl;
  cout << "UUID: " << item.get<UUID>() << endl;
  
  auto child = item.addChild("Child");
  cout << "Child: " << child.get<Tag>().tag << endl;
  //cout << "Parent: " << child.get<Parent>().parent.get<Tag>().tag << endl;
  cout << "Parent: " << child.parent().get<Tag>().tag << endl;
 
  return SDL_APP_CONTINUE;
}

SDL_AppResult BaseGameApp::iterate() {
  return SDL_APP_CONTINUE;
}

SDL_AppResult BaseGameApp::handle(SDL_Event *event) {
  return SDL_APP_CONTINUE;
}

void BaseGameApp::quit(SDL_AppResult result) {
  SDL_Quit();
}
