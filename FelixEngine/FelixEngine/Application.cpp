//
//  Application.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 6/13/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Application.h"


using namespace fx;

FileSystem* FileSystem::instance = nullptr;
Graphics* Graphics::instance = nullptr;


Application::Application() {
  printf("Created Application\n");
}

Application::~Application() {
  
}

void Application::start() {
  printf("Application Start");
}