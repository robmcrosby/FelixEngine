//
//  FelixEngine.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/2/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "FelixEngine.h"

#include "System.h"
#include "Scene.h"
#include "Platform.h"

#include "TaskingSystem.h"
#include "GraphicSystem.h"

#include <SDL2/SDL.h>


using namespace fx;
using namespace std;

FelixEngine* FelixEngine::Instance()
{
  static FelixEngine engine;
  return &engine;
}

System* FelixEngine::GetSystem(SYSTEM_TYPE type)
{
  return Instance()->getSystem(type);
}

TaskingSystem* FelixEngine::GetTaskingSystem()
{
  return static_cast<TaskingSystem*>(GetSystem(SYSTEM_TASKING));
}

GraphicSystem* FelixEngine::GetGraphicSystem()
{
  return static_cast<GraphicSystem*>(GetSystem(SYSTEM_GRAPHICS));
}

Scene* FelixEngine::GetScene(const std::string &name)
{
  return Instance()->getScene(name);
}

FelixEngine::FelixEngine(): mRefreshRate(16)
{
}

FelixEngine::~FelixEngine()
{
  clearScenes();
  clearSystems();
}

bool FelixEngine::init(const std::string &settingsFile)
{
  bool success = false;
  XMLTree::XMLTree tree;
  if (tree.loadFile(Platform::GetResourcePath()+settingsFile) && !tree.isEmpty())
  {
    const XMLTree::Node *settingsNode = *tree.begin();
    if (settingsNode->hasSubNode("Systems"))
      success = loadSystems(*settingsNode->subNode("Systems"));
  }
  
  return success;
}

bool FelixEngine::loadScene(const string &sceneFile)
{
  bool success = false;
  XMLTree::XMLTree tree;
  if (tree.loadFile(Platform::GetResourcePath()+sceneFile) && !tree.isEmpty())
  {
    Scene *scene = new Scene();
    success = scene->setToXml(*tree.begin()) && scene->init();
    if (success)
      addScene(scene);
    else
      delete scene;
  }
  return success;
}

void FelixEngine::addScene(Scene *scene)
{
  if (scene)
  {
    Scene *prev = getScene(scene->name());
    if (prev && prev != scene)
      delete prev;
    mScenes[scene->name()] = scene;
  }
}

void FelixEngine::clearScenes()
{
  for (map<string, Scene*>::iterator itr = mScenes.begin(); itr != mScenes.end(); ++itr)
    delete itr->second;
  mScenes.clear();
}

void FelixEngine::deleteScene(const std::string &name)
{
  if (mScenes.count(name))
  {
    delete mScenes.at(name);
    mScenes.erase(name);
  }
}

Scene* FelixEngine::getScene(const std::string &name)
{
  return mScenes.count(name) ? mScenes.at(name) : nullptr;
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
    updateFrame();
  }
  
  SDL_Quit();
  return 0;
}

bool FelixEngine::loadSystems(const XMLTree::Node &node)
{
  bool success = true;
  int  sdlInitFlags = 0;
  
  for (XMLTree::const_iterator itr = node.begin(); itr != node.end(); ++itr)
  {
    System *system = System::Create((*itr)->element());
    if (system)
    {
      success &= system->setToXml(*itr);
      sdlInitFlags |= system->getSDLInitFlags();
      addSystem(system);
    }
  }
  
  if (success && SDL_Init(sdlInitFlags) < 0)
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

void FelixEngine::clearSystems()
{
  for (map<SYSTEM_TYPE, System*>::iterator itr = mSystems.begin(); itr != mSystems.end(); ++itr)
    delete itr->second;
  mSystems.clear();
}

System* FelixEngine::getSystem(SYSTEM_TYPE type)
{
  return mSystems.count(type) ? mSystems.at(type) : nullptr;
}

void FelixEngine::updateFrame()
{
  for (map<SYSTEM_TYPE, System*>::iterator itr = mSystems.begin(); itr != mSystems.end(); ++itr)
    itr->second->getUpdateDelegate()(nullptr);
  for (map<string, Scene*>::iterator itr = mScenes.begin(); itr != mScenes.end(); ++itr)
    itr->second->update();
}
