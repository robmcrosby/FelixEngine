//
//  GpuContext.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/9/26.
//

#include "GpuContext.hpp"


using namespace std;
namespace Felix {

GpuContextPtr GpuContext::create() {
  GpuContextPtr context = make_shared<GpuContext>();
  return context;
}

GpuContext::GpuContext() {
  
}

GpuContext::~GpuContext() {
  
}

bool GpuContext::init() {
  // Create the main window
  mWindow = SDL_CreateWindow("SDLGame", 640, 480, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_BORDERLESS);
  if (!mWindow) {
    cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
    return false;
  }
  
  return true;
}

} /* Felix */
