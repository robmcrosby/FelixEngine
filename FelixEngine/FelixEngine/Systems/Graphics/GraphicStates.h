//
//  GraphicStates.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/19/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef GraphicStates_h
#define GraphicStates_h

#include "Matrix.h"

#define MAX_COLOR_ATTACHEMENTS 4


namespace fx {
  
  enum LOAD_ACTION {
    LOAD_LAST,
    LOAD_NONE,
    LOAD_CLEAR,
  };
  
  enum STORE_ACTION {
    STORE_SAVE,
    STORE_DISCARD,
  };
  
  enum CULL_MODE {
    CULL_NONE,
    CULL_FRONT,
    CULL_BACK,
  };
  
  struct AttachmentState {
    LOAD_ACTION  loadAction;
    STORE_ACTION storeAction;
    vec4 clearColor;
    
    AttachmentState(): loadAction(LOAD_LAST), storeAction(STORE_SAVE) {
      clearColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
  };
}

#endif /* GraphicStates_h */
