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
#include "XMLTree.h"

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
  
  struct ActionState {
    LOAD_ACTION  loadAction;
    STORE_ACTION storeAction;
    vec4 clearColor;
    
    ActionState(): loadAction(LOAD_LAST), storeAction(STORE_SAVE) {
      clearColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
  };
  
  enum CULL_MODE {
    CULL_NONE,
    CULL_FRONT,
    CULL_BACK,
  };
  
  enum DEPTH_FLAGS {
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
  
  struct DepthState {
    int flags;
    
    DepthState(int flags = 0): flags(flags) {}
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
    
    void enableDefaultTesting() {
      enableTesting();
      enableWriting();
      setFunction(DEPTH_TEST_LESS);
    }
    
    void setFunction(int function) {
      flags = (function & DEPTH_TEST_MASK) | (flags & DEPTH_ENABLE_MASK);
    }
    
    bool loadXml(const XMLTree::Node &node)
    {
      setWriting(node.attributeAsBoolean("write"));
      if (node.hasAttribute("function")) {
        std::string func = node.attribute("function");
        std::transform(func.begin(), func.end(), func.begin(), ::tolower);
        
        enableTesting();
        if (func == "less_equal")
          setFunction(DEPTH_TEST_LESS_EQ);
        else if (func == "greater_equal")
          setFunction(DEPTH_TEST_GREATER_EQ);
        else if (func == "less")
          setFunction(DEPTH_TEST_LESS);
        else if (func == "greater")
          setFunction(DEPTH_TEST_GREATER);
        else if (func == "equal")
          setFunction(DEPTH_TEST_EQUAL);
        else if (func == "never")
          setFunction(DEPTH_TEST_NEVER);
        else
          setFunction(DEPTH_TEST_ALWAYS);
      }
      else {
        disableTesting();
        setFunction(DEPTH_TEST_ALWAYS);
      }
      return true;
    }
    
    bool enabled() const {return writingEnabled() || testingEnabled();}
    bool writingEnabled() const {return flags & DEPTH_ENABLE_WRITING;}
    bool testingEnabled() const {return flags & DEPTH_ENABLE_TESTING;}
    int  function()  const {return flags & DEPTH_TEST_MASK;}
  };
  
  
  enum COORD_TYPE
  {
    COORD_CLAMP_EDGE  = 0,
    COORD_REPEAT      = 1,
    COORD_MIRROR      = 2,
    COORD_CLAMP_ZERO  = 3,
  };
  
  enum FILTER_TYPE
  {
    FILTER_NEAREST = 0,
    FILTER_LINEAR  = 1,
  };
  
  enum SAMPLER_FLAGS
  {
    SAMPLER_SAMPLES_MASK = 0x00ff,
    SAMPLER_MAX_SAMPLES  = 32,
    
    SAMPLER_COORD_S_OFFSET = 8,
    SAMPLER_COORD_T_OFFSET = 10,
    SAMPLER_COORD_R_OFFSET = 12,
    
    SAMPLER_COORD_S_MASK   = 0x0300,
    SAMPLER_COORD_T_MASK   = 0x0C00,
    SAMPLER_COORD_R_MASK   = 0x3000,
    
    SAMPLER_FILTER_MIN_OFFSET = 14,
    SAMPLER_FILTER_MAG_OFFSET = 15,
    SAMPLER_FILTER_MIP_OFFSET = 16,
    
    SAMPLER_FILTER_MIN_MASK = 0x004000,
    SAMPLER_FILTER_MAG_MASK = 0x008000,
    SAMPLER_FILTER_MIP_MASK = 0x010000,
    
    SAMPLER_MIP_ENABLED   = 0x020000,
    SAMPLER_UN_NORMALIZED = 0x040000
  };
  
  struct SamplerState {
    int flags;
    
    SamplerState(int flags = 0): flags(flags) {}
    
    void setSamples(int samples)
    {
      samples = samples < 1 ? 1 : samples > SAMPLER_MAX_SAMPLES ? SAMPLER_MAX_SAMPLES : samples;
      flags = (flags & ~SAMPLER_SAMPLES_MASK) | ((samples - 1) & SAMPLER_SAMPLES_MASK);
    }
    int samples() const {return (flags & ~SAMPLER_SAMPLES_MASK) + 1;}
    
    void setSCoord(COORD_TYPE coord) {flags = (flags & ~SAMPLER_COORD_S_MASK) | (coord << SAMPLER_COORD_S_OFFSET);}
    COORD_TYPE sCoord() const {return (COORD_TYPE)((flags & SAMPLER_COORD_S_MASK) >> SAMPLER_COORD_S_OFFSET);}
    
    void setTCoord(COORD_TYPE coord) {flags = (flags & ~SAMPLER_COORD_T_MASK) | (coord << SAMPLER_COORD_T_OFFSET);}
    COORD_TYPE tCoord() const {return (COORD_TYPE)((flags & SAMPLER_COORD_T_MASK) >> SAMPLER_COORD_T_OFFSET);}
    
    void setRCoord(COORD_TYPE coord) {flags = (flags & ~SAMPLER_COORD_R_MASK) | (coord << SAMPLER_COORD_R_OFFSET);}
    COORD_TYPE rCoord() const {return (COORD_TYPE)((flags & SAMPLER_COORD_R_MASK) >> SAMPLER_COORD_R_OFFSET);}
    
    void setMinFilter(FILTER_TYPE filter) {flags = (flags & ~SAMPLER_FILTER_MIN_MASK ) | (filter << SAMPLER_FILTER_MIN_OFFSET);}
    FILTER_TYPE minFilter() const {return (FILTER_TYPE)((flags & SAMPLER_FILTER_MIN_MASK) >> SAMPLER_FILTER_MIN_OFFSET);}
    
    void setMagFilter(FILTER_TYPE filter) {flags = (flags & ~SAMPLER_FILTER_MAG_MASK ) | (filter << SAMPLER_FILTER_MAG_OFFSET);}
    FILTER_TYPE magFilter() const {return (FILTER_TYPE)((flags & SAMPLER_FILTER_MAG_MASK) >> SAMPLER_FILTER_MAG_OFFSET);}
    
    void setMipFilter(FILTER_TYPE filter) {flags = (flags & ~SAMPLER_FILTER_MIP_MASK ) | (filter << SAMPLER_FILTER_MIP_OFFSET);}
    FILTER_TYPE mipFilter() const {return (FILTER_TYPE)((flags & SAMPLER_FILTER_MIP_MASK) >> SAMPLER_FILTER_MIP_OFFSET);}
    
    void enableMipMapping() {flags |= SAMPLER_MIP_ENABLED;}
    void disableMipMapping() {flags &= ~SAMPLER_MIP_ENABLED;}
    bool mipMappingEnabled() const {return flags & SAMPLER_MIP_ENABLED;}
    
    void setNormalized() {flags &= ~SAMPLER_UN_NORMALIZED;}
    void setUnNormalized() {flags |= SAMPLER_UN_NORMALIZED;}
    bool isNormalized() const {return !(flags & SAMPLER_UN_NORMALIZED);}
  };
}

#endif /* GraphicStates_h */
