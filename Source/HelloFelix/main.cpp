//
//  main.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/1/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//


#include <FelixEngine/FelixEngine.h>
#include <stdio.h>

#include <FelixEngine/ImageLoader.h>

//#define SETTINGS_FILE "SettingsMetal.xml"
//#define SCENE_FILE "TriangleSceneMetal.xml"
//#define SCENE_FILE "TextureSceneMetal.xml"
//#define SCENE_FILE "FrameSceneMetal.xml"

#define SETTINGS_FILE "SettingsGL41.xml"
//#define SCENE_FILE "TriangleSceneGL41.xml"
//#define SCENE_FILE "TextureSceneGL41.xml"
//#define SCENE_FILE "FrameSceneGL41.xml"
#define SCENE_FILE "DepthSceneGL41.xml"


using namespace std;

int main( int argc, char* args[] )
{
  fx::FelixEngine *engine = fx::FelixEngine::Instance();
  
  if (!engine->init(SETTINGS_FILE))
  {
    cerr << "Error Initalizing the Engine" << endl;
    return 1;
  }
  
  if (!engine->loadScene(SCENE_FILE))
  {
    cerr << "Error Loading the Scene" << endl;
    return 1;
  }
  
  return engine->runLoop();
}