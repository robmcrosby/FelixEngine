//
//  main.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/1/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include <SDL2/SDL.h>
#include <stdio.h>


bool init();

SDL_Window* gWindow = NULL;

bool init()
{
  bool success = true;
  
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    success = false;
  }
  else
  {
    gWindow = SDL_CreateWindow( "Hello Felix", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN );
    if( gWindow == NULL )
    {
      printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
      success = false;
    }
  }
  
  return success;
}

int main( int argc, char* args[] )
{
  if (!init())
    printf("Failed to initialize!\n");
  else
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
      SDL_UpdateWindowSurface(gWindow);
    }
  }
  
  SDL_DestroyWindow( gWindow );
  gWindow = NULL;
  
  SDL_Quit();
  
  return 0;
}