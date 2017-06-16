//
//  MetalGraphics.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/14/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Graphics.h"
//#include <objc/objc-runtime.h>


#ifdef __OBJC__
@class UIView;
#else
typedef struct objc_object UIView;
#endif


namespace fx {
  
  class MetalGraphicsData;
  
  /** iOS File System */
  class MetalGraphics: public Graphics {
  private:
    MetalGraphicsData *_data;
    
  public:
    MetalGraphics();
    virtual ~MetalGraphics();
    
    bool initalize(UIView *view);
  };
  
}
