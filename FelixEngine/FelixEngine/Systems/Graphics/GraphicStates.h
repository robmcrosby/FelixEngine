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
  
  struct ActionState {
    LOAD_ACTION  loadAction;
    STORE_ACTION storeAction;
    vec4 clearColor;
    
    ActionState(): loadAction(LOAD_LAST), storeAction(STORE_SAVE) {
      clearColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
  };
  
  
  enum DEPTH_FLAGS
  {
    DEPTH_ENABLE_MASK     = 0x0f, // 0000 1111
    DEPTH_ENABLE_WRITING  = 0x01, // 0000 0001
    DEPTH_ENABLE_TESTING  = 0x02, // 0000 0010
    
    DEPTH_TEST_MASK       = 0xf0, // 1111 0000
    DEPTH_TEST_ALWAYS     = 0x00, // 0000 0000
    DEPTH_TEST_NEVER      = 0x70, // 0111 0000
    
    DEPTH_TEST_EQUAL      = 0x50, // 0101 0000
    DEPTH_TEST_LESS       = 0x60, // 0110 0000
    DEPTH_TEST_GREATER    = 0x30, // 0011 0000
    
    DEPTH_TEST_LESS_EQ    = 0x40, // 0100 0000
    DEPTH_TEST_GREATER_EQ = 0x10, // 0001 0000
  };
  
  struct DepthStencilState
  {
    int flags;
    
    DepthStencilState(int flags = 0): flags(flags) {}
    void setWriting(bool writing = true) {
      if (writing)
        flags |= DEPTH_ENABLE_WRITING;
      else
        flags &= ~DEPTH_ENABLE_WRITING;
    }
    void enableWriting() {setWriting(true);}
    void disableWriting() {setWriting(false);}
    
    void setTesting(bool testing = true) {
      if (testing)
        flags |= DEPTH_ENABLE_TESTING;
      else
        flags &= ~DEPTH_ENABLE_TESTING;
    }
    void enableTesting() {setTesting(true);}
    void disableTesting() {setTesting(false);}
    
    void setFunction(int function) {
      flags = (function & DEPTH_TEST_MASK) | (flags & DEPTH_ENABLE_MASK);
    }
    
    bool enabled() const {return writingEnabled() || testingEnabled();}
    bool writingEnabled() const {return flags & DEPTH_ENABLE_WRITING;}
    bool testingEnabled() const {return flags & DEPTH_ENABLE_TESTING;}
    int  function()  const {return flags & DEPTH_TEST_MASK;}
  };
}

#endif /* GraphicStates_h */
