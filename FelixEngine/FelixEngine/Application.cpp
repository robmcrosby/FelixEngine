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
  
}

Application::~Application() {
  
}

void Application::initalize() {
  
}

void Application::update() {
  
}

void Application::render() {
  
}

void Application::processFrame() {
  update();
  
  if (_graphics != nullptr) {
    _graphics->nextFrame();
    render();
    _graphics->presentFrame();
  }
}
