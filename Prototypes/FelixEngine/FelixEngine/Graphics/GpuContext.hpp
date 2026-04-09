//
//  GpuContext.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/9/26.
//

#ifndef GpuContext_hpp
#define GpuContext_hpp

#include <SDL3/SDL.h>
#include <iostream>
#include <memory>

namespace Felix {
  
class GpuContext;
typedef std::shared_ptr<GpuContext> GpuContextPtr;

class GpuContext {
private:
  SDL_Window* mWindow;
  
public:
  GpuContext();
  ~GpuContext();
  
  bool init();
  
public:
  static GpuContextPtr create();
};

} /* Felix */

#endif /* GpuContext_hpp */
