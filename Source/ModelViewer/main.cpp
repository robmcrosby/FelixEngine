//
//  main.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 11/13/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include <FelixEngine/FelixEngine.h>


#if __IPHONEOS__
//#define SETTINGS_FILE "SettingsGLES2.xml"
#define SETTINGS_FILE "SettingsGLES3.xml"
//#define SETTINGS_FILE "SettingsMetal_IOS.xml"
#else
//#define SETTINGS_FILE "SettingsGL41.xml"
#define SETTINGS_FILE "SettingsMetal.xml"
#endif

#define SCENE_FILE "MainScene.xml"


using namespace std;


int main(int argc, char* args[])
{
  fx::FelixEngine *engine = fx::FelixEngine::Instance();
  
  if (!engine->init(SETTINGS_FILE))
  {
    cerr << "Error Initalizing the Engine" << endl;
    return 1;
  }
  
//  fx::File documents = fx::FileSystem::GetDocuments()+"BunnyScene/Meshes";
//  fx::Directory dir;
//  documents.load(dir);
//  for (fx::Directory::iterator itr = dir.begin(); itr != dir.end(); ++itr)
//    cout << *itr << endl;
  
//  if (!engine->loadScene(fx::FileSystem::GetDocuments()+"BunnyScene/Scene.xml"))
  if (!engine->loadScene(SCENE_FILE))
  {
    cerr << "Error Loading the Scene" << endl;
    return 1;
  }
  
  return engine->runLoop();
}