//
//  FelixEngine.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/2/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "FelixEngine.h"
#include <SDL2/SDL.h>

using namespace fx;
using namespace std;


FelixEngine* FelixEngine::Instance()
{
  static FelixEngine engine;
  return &engine;
}

FelixEngine::FelixEngine(): mWindow(0)
{
  cout << "Created Felix Engine" << endl;
}

FelixEngine::~FelixEngine()
{
  
}

bool FelixEngine::init(const std::string &settingsFile)
{
  bool success = true;
  
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    success = false;
  }
  else
  {
    mWindow = SDL_CreateWindow("Felix Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    if( mWindow == NULL )
    {
      printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
      success = false;
    }
  }
  
  return success;
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
    SDL_UpdateWindowSurface(mWindow);
  }

  SDL_DestroyWindow(mWindow);
  mWindow = nullptr;
  
  SDL_Quit();
  return 0;
}
