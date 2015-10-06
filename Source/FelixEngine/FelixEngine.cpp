//
//  FelixEngine.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/2/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "FelixEngine.h"
#include "GraphicSystem.h"
#include <SDL2/SDL.h>

using namespace fx;
using namespace std;


FelixEngine* FelixEngine::Instance()
{
  static FelixEngine engine;
  return &engine;
}

FelixEngine::FelixEngine(): mGraphicSystem(0)
{
  cout << "Created Felix Engine" << endl;
}

FelixEngine::~FelixEngine()
{
  
}

bool FelixEngine::init(const std::string &settingsFile)
{
  Uint32 initFlags = SDL_INIT_VIDEO;
  
  if (SDL_Init(initFlags) < 0)
  {
    cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
    return false;
  }
  
  mGraphicSystem = GraphicSystem::CreateOpenGLContext();
  if (!mGraphicSystem)
  {
    cerr << "Error creating GpuSystem" << endl;
    return false;
  }
  else
    mSystems[SYSTEM_GRAPHICS] = mGraphicSystem;
  
  return true;
}

bool FelixEngine::loadScene(const std::string &sceneFile)
{
  return true;
}

int FelixEngine::runLoop()
{
  bool quit = false;
  SDL_Event event;
  
  while (!quit)
  {
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
        quit = true;
      else
        updateFrame();
    }
  }
  
  SDL_Quit();
  return 0;
}

void FelixEngine::updateFrame()
{
  for (map<SYSTEM_TYPE, System*>::iterator itr = mSystems.begin(); itr != mSystems.end(); ++itr)
    itr->second->update();
}
