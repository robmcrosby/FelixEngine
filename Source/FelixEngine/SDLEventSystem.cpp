//
//  SDLEventSystem.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 11/3/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "SDLEventSystem.h"
#include "FelixEngine.h"
#include "Event.h"

using namespace fx;
using namespace std;

SDLEventSystem::SDLEventSystem(): mEngine(FelixEngine::Instance())
{
  setEventFlags(EVENT_UPDATE);
}

SDLEventSystem::~SDLEventSystem()
{
}

bool SDLEventSystem::setToXml(const XMLTree::Node *node)
{
  return true;
}

void SDLEventSystem::pollEvents()
{
  if (mUpdateMutex.tryLock())
  {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
        mEngine->exit();
    }
    mUpdateMutex.unlock();
  }
}

bool SDLEventSystem::init()
{
  return true;
}
