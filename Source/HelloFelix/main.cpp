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

using namespace std;

int main( int argc, char* args[] )
{
  fx::FelixEngine *engine = fx::FelixEngine::Instance();
  
  if (!engine->init("SettingsGL41.xml"))
  {
    cerr << "Error Initalizing the Engine" << endl;
    return 1;
  }
  
  if (!engine->loadScene("TextureSceneGL41.xml"))
  {
    cerr << "Error Loading the Scene" << endl;
    return 1;
  }
  
  return engine->runLoop();
}