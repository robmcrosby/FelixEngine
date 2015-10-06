//
//  FelixEngine.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/2/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "FelixEngine.h"
#include "System.h"
#include "Platform.h"
#include <SDL2/SDL.h>

using namespace fx;
using namespace std;


FelixEngine* FelixEngine::Instance()
{
  static FelixEngine engine;
  return &engine;
}

FelixEngine::FelixEngine()
{
  cout << "Created Felix Engine" << endl;
}

FelixEngine::~FelixEngine()
{
  
}

bool FelixEngine::init(const std::string &settingsFile)
{
  XMLTree::XMLTree tree;
  if (tree.loadFile(Platform::GetResourcePath()+settingsFile))
  {
    const XMLTree::Node &settingsNode = **tree.begin();
    if (settingsNode.hasSubNode("Systems"))
      return loadSystems(*settingsNode.subNode("Systems"));
  }
  
  return false;
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

bool FelixEngine::loadSystems(const XMLTree::Node &node)
{
  bool success = true;
  int  initFlags = 0;
  
  for (XMLTree::const_iterator itr = node.begin(); itr != node.end(); ++itr)
  {
    System *system = System::Create((*itr)->element());
    if (system)
    {
      success &= system->setToXml(*itr);
      initFlags |= system->getInitFlags();
      addSystem(system);
    }
  }
  
  if (success && SDL_Init(initFlags) < 0)
  {
    cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
    success = false;
  }
  
  for (map<SYSTEM_TYPE, System*>::iterator itr = mSystems.begin(); success && itr != mSystems.end(); ++itr)
    success &= itr->second->init();
  
  return success;
}

void FelixEngine::addSystem(System *system)
{
  if (system)
  {
    if (mSystems.count(system->type()))
      delete mSystems.at(system->type());
    mSystems[system->type()] = system;
  }
}

bool FelixEngine::loasScene(const XMLTree::Node &node)
{
  bool success = true;
  return success;
}

void FelixEngine::updateFrame()
{
  for (map<SYSTEM_TYPE, System*>::iterator itr = mSystems.begin(); itr != mSystems.end(); ++itr)
    itr->second->update();
}
