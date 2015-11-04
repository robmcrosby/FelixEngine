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

#include <SDL2/SDL.h>


using namespace fx;
using namespace std;

FelixEngine* FelixEngine::Instance()
{
  static FelixEngine engine;
  return &engine;
}

FelixEngine::FelixEngine(): mShutdown(0)
{
  setEventFlags(EVENT_APP_QUIT);
}

FelixEngine::~FelixEngine()
{
  clearScenes();
  clearSystems();
}

void FelixEngine::handle(const Event &event)
{
  if (event == EVENT_APP_QUIT)
    exit();
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
    addHandler(scene);
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

int FelixEngine::runLoop()
{
  while (!mShutdown)
  {
    notify(Event(EVENT_APP_UPDATE, DISPATCH_MULTIPLE));
    
    EventSystem *eventSystem = GetEventSystem();
    if (eventSystem)
      eventSystem->pollEvents();
    
    GraphicSystem *graphicSystem = getGraphicSystem();
    if (graphicSystem)
      graphicSystem->render();
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
  
  // Add the default tasking system if there is none found.
  if (!getTaskingSystem())
    addSystem(System::Create("TaskingSystem"));
  
  // Add the default event system if there is none found.
  if (!getEventSystem())
    addSystem(System::Create("SDLEventSystem"));
  getEventSystem()->addHandler(this);
  
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
    addHandler(system);
  }
}

void FelixEngine::clearSystems()
{
  for (map<SYSTEM_TYPE, System*>::iterator itr = mSystems.begin(); itr != mSystems.end(); ++itr)
    delete itr->second;
  mSystems.clear();
}
