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
TrackerSystem* TrackerSystem::instance = nullptr;


Application::Application() {
  
}

Application::~Application() {
  
}

void Application::initalize() {
  
}

void Application::handle(const Event &event) {
  if (event.catagory == EVENT_INPUT) {
    if (event.type == INPUT_TOUCH_DOWN) {
      for (int i = 0; i < event.message.size(); ++i) {
        const Touch &touch = (const Touch&)event.message[i];
        std::cout << touch.location << std::endl;
      }
    }
  }
}

void Application::update(float td) {
  
}

void Application::render() {
  
}

void Application::willEnterBackground() {
  
}

void Application::willEnterForeground() {
  
}

void Application::willTerminate() {
  
}

void Application::processFrame(float td) {
  if (_graphics != nullptr) {
    _graphics->nextFrame();
    update(td);
    render();
    _graphics->presentFrame();
  }
}
