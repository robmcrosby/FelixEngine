//
//  main.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/1/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//


#include <FelixEngine/FelixEngine.h>
#include <stdio.h>

using namespace std;

int main( int argc, char* args[] )
{
  fx::FelixEngine engine;
  
  if (!engine.init())
  {
    cerr << "Error Initalizing the Engine" << endl;
    return 1;
  }
  
  return engine.runLoop();
}