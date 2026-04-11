//
//  SdlEvent.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/10/26.
//

#ifndef SdlEvent_hpp
#define SdlEvent_hpp

#include <SDL3/SDL.h>
#include <iostream>


namespace Felix {

struct SdlEvent {
  SDL_Event* event;
  
  SdlEvent(): event(NULL) {}
  SdlEvent(SDL_Event* e): event(e) {}
  SdlEvent(const SdlEvent&) = default;
  
  operator SDL_Event* () const { return event; }
};

} /* Felix */

#endif /* SdlEvent_hpp */
