//
//  FelixEngine.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/2/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "FelixEngine.h"
#include "GpuSystem.h"
#include <SDL2/SDL.h>

using namespace fx;
using namespace std;


FelixEngine* FelixEngine::Instance()
{
  static FelixEngine engine;
  return &engine;
}

FelixEngine::FelixEngine(): mGpuSystem(0)
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
  
  mGpuSystem = GpuSystem::CreateOpenGLSystem();
  if (!mGpuSystem)
  {
    cerr << "Error creating GpuSystem" << endl;
    return false;
  }
  
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
    }
    //SDL_UpdateWindowSurface(mWindow);
  }

  //SDL_DestroyWindow(mWindow);
  //mWindow = nullptr;
  
  SDL_Quit();
  return 0;
}
