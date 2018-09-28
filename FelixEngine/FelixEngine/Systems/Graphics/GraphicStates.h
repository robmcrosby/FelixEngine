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
    LOAD_NONE  = 0x00,
    LOAD_LAST  = 0x01,
    LOAD_CLEAR = 0x02,
  };
  
  enum STORE_ACTION {
    STORE_DISCARD = 0x00,
    STORE_SAVE    = 0x04,
  };
  
  struct ActionState {
    LOAD_ACTION  loadAction;
    STORE_ACTION storeAction;
    vec4 clearColor;
    
    ActionState(): loadAction(LOAD_LAST), storeAction(STORE_SAVE) {
      clearColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    uint32_t key() const {return loadAction | storeAction;}
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
    DepthState(const XMLTree::Node &node): flags(0) {loadXml(node);}
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
    void disable() {
      disableWriting();
      disableTesting();
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
  
  
  /**
   * 0000 0000 0000 0000 0000 0000 #### ####
   */
  enum BLEND_EQ
  {
    BLEND_EQ_MASK         = 0xff,
    BLEND_EQ_COMP_MASK    = 0xf,
    
    BLEND_EQ_NONE         = 0x0,
    BLEND_EQ_ADD          = 0x1,
    BLEND_EQ_SUBTRACT     = 0x2,
    BLEND_EQ_REV_SUBTRACT = 0x3,
    BLEND_EQ_MIN          = 0x4,
    BLEND_EQ_MAX          = 0x5,
    
    BLEND_EQ_COLOR_SHIFT  = 0,
    BLEND_EQ_ALPHA_SHIFT  = 4,
  };
  
  /**
   * 0000 0000 #### #### #### #### 0000 0000
   */
  enum BLEND_INPUTS {
    BLEND_INPUT_MASK        = 0xffff00,
    BLEND_INPUT_COMP_MASK   = 0xf,
    
    BLEND_INPUT_SRC_COLOR   = 0x1, // 0001
    BLEND_INPUT_SRC_ALPHA   = 0x2, // 0010
    BLEND_INPUT_DST_COLOR   = 0x3, // 0011
    BLEND_INPUT_DST_ALPHA   = 0x4, // 0100
    BLEND_INPUT_CONST_COLOR = 0x5, // 0101
    BLEND_INPUT_CONST_ALPHA = 0x6, // 0110
    
    BLEND_INPUT_ONE_MINUS   = 0x8, // 1000
    
    BLEND_INPUT_ZERO        = 0x8, // 1000
    BLEND_INPUT_ONE         = 0x0, // 0000
    
    BLEND_INPUT_SRC_ALPHA_SATURATE = 0xf, // 1111
    
    BLEND_INPUT_SRC_COLOR_SHIFT  = 8,
    BLEND_INPUT_DEST_COLOR_SHIFT = 12,
    BLEND_INPUT_SRC_ALPHA_SHIFT  = 16,
    BLEND_INPUT_DEST_ALPHA_SHIFT = 20,
  };
  
  struct BlendState
  {
    int   flags;
    RGBAf color;
    
    BlendState(int f = 0): flags(f) {}
    BlendState(const XMLTree::Node &node): flags(0) {loadXml(node);}
    
    void enableDefaultBlending()
    {
      setEquation(BLEND_EQ_ADD);
      setSrc(BLEND_INPUT_SRC_ALPHA);
      setDst(BLEND_INPUT_ONE_MINUS | BLEND_INPUT_SRC_ALPHA);
    }
    void disable() {setEquation(BLEND_EQ_NONE);}
    
    void setEquation(int func, int shift = BLEND_EQ_COLOR_SHIFT) {
      flags = (flags & ~(BLEND_EQ_COMP_MASK << shift)) | ((func << shift) & (BLEND_EQ_COMP_MASK << shift));
    }
    void setEquationAlpha(int func) {setEquation(func, BLEND_EQ_ALPHA_SHIFT);}
    
    void setEquation(const std::string &str, int shift = BLEND_EQ_COLOR_SHIFT) {setEquation(getEquationFlag(str), shift);}
    void setEquationAlpha(const std::string &str) {setEquation(str, BLEND_EQ_ALPHA_SHIFT);}
    
    void setInput(int input, int shift)
    {
      int mask = BLEND_INPUT_COMP_MASK << shift;
      flags = (flags & ~mask) | ((input << shift) & mask);
    }
    void setSrc(int input) {setInput(input, BLEND_INPUT_SRC_COLOR_SHIFT);}
    void setDst(int input) {setInput(input, BLEND_INPUT_DEST_COLOR_SHIFT);}
    void setSrcAlpha(int input) {setInput(input, BLEND_INPUT_SRC_ALPHA_SHIFT);}
    void setDstAlpha(int input) {setInput(input, BLEND_INPUT_DEST_ALPHA_SHIFT);}
    
    void setSrc(const std::string &str) {setSrc(getInputFlag(str));}
    void setDst(const std::string &str) {setDst(getInputFlag(str));}
    void setSrcAlpha(const std::string &str) {setSrcAlpha(getInputFlag(str));}
    void setDstAlpha(const std::string &str) {setDstAlpha(getInputFlag(str));}
    
    int equation() const {return (flags >> BLEND_EQ_COLOR_SHIFT) & BLEND_EQ_COMP_MASK;}
    int equationAlpha() const {return (flags >> BLEND_EQ_ALPHA_SHIFT) & BLEND_EQ_COMP_MASK;}
    
    int src() const {return (flags >> BLEND_INPUT_SRC_COLOR_SHIFT) & BLEND_INPUT_COMP_MASK;}
    int dst() const {return (flags >> BLEND_INPUT_DEST_COLOR_SHIFT) & BLEND_INPUT_COMP_MASK;}
    int srcAlpha() const {return (flags >> BLEND_INPUT_SRC_ALPHA_SHIFT) & BLEND_INPUT_COMP_MASK;}
    int dstAlpha() const {return (flags >> BLEND_INPUT_DEST_ALPHA_SHIFT) & BLEND_INPUT_COMP_MASK;}
    
    bool enabled() const {return (flags >> BLEND_EQ_COLOR_SHIFT) & BLEND_EQ_MASK;}
    bool seperate() const {return (flags >> BLEND_EQ_ALPHA_SHIFT) & BLEND_EQ_COMP_MASK;}
    
    static int getInputFlag(const std::string &str)
    {
      if (str == "saturate")
        return BLEND_INPUT_SRC_ALPHA_SATURATE;
      if (str == "0")
        return BLEND_INPUT_ZERO;
      if (str == "1")
        return BLEND_INPUT_ONE;
      
      int mod = (str.find("1-") == 0) ? BLEND_INPUT_ONE_MINUS : 0;
      if (str.find("src") != std::string::npos)
        return mod | ((str.find("alpha") != std::string::npos) ? BLEND_INPUT_SRC_ALPHA : BLEND_INPUT_SRC_COLOR);
      if (str.find("dst") != std::string::npos)
        return mod | ((str.find("alpha") != std::string::npos) ? BLEND_INPUT_DST_ALPHA : BLEND_INPUT_DST_COLOR);
      return mod | ((str.find("alpha") != std::string::npos) ? BLEND_INPUT_CONST_ALPHA : BLEND_INPUT_CONST_COLOR);
    }
    
    static int getEquationFlag(const std::string &str)
    {
      if (str == "add")
        return BLEND_EQ_ADD;
      if (str == "subtract")
        return BLEND_EQ_SUBTRACT;
      if (str == "reverse")
        return BLEND_EQ_REV_SUBTRACT;
      if (str == "min")
        return BLEND_EQ_MIN;
      if (str == "max")
        return BLEND_EQ_MAX;
      return 0;
    }
    
    bool loadXml(const XMLTree::Node &node)
    {
      if (node.hasAttribute("equation"))
        setEquation(node.attribute("equation"));
      else
        setEquation(BLEND_EQ_ADD);
      
      if (node.hasAttribute("src"))
        setSrc(node.attribute("src"));
      else
        setSrc(BLEND_INPUT_SRC_ALPHA);
      
      if (node.hasAttribute("dst"))
        setDst(node.attribute("dst"));
      else
        setDst(BLEND_INPUT_ONE_MINUS | BLEND_INPUT_SRC_ALPHA);
      
      if (node.hasAttribute("color"))
        color = node.attribute("color");
      else if (node.hasSubNode("Color"))
        color = node.subContents("Color");
      else
        color = RGBAf(0.0f, 0.0f, 0.0f, 1.0f);
      
      const XMLTree::Node *alpha = node.subNode("Alpha");
      if (alpha)
      {
        if (alpha->hasAttribute("equation"))
          setEquationAlpha(alpha->attribute("equation"));
        else
          setEquationAlpha(equation());
        
        if (alpha->hasAttribute("src"))
          setSrcAlpha(alpha->attribute("src"));
        else
          setSrcAlpha(src());
        
        if (alpha->hasAttribute("dst"))
          setDstAlpha(alpha->attribute("dst"));
        else
          setDstAlpha(dst());
      }
      return true;
    }
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
    
    SAMPLER_FILTER_MIN_OFFSET = 14,  //0100 0000 0000 0000
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
    SamplerState(const XMLTree::Node &node): flags(0) {loadXml(node);}
    
    static COORD_TYPE parseCoord(const std::string &coord) {
      if (coord == "repeat")
        return COORD_REPEAT;
      if (coord == "mirror")
        return COORD_MIRROR;
      if (coord == "zero")
        return COORD_CLAMP_ZERO;
      return COORD_CLAMP_EDGE;
    }
    
    static FILTER_TYPE parseFilter(const std::string &filter) {
      if (filter == "linear")
        return FILTER_LINEAR;
      return FILTER_NEAREST;
    }
    
    bool loadXml(const XMLTree::Node &node) {
      flags = 0;
      setSamples(node.attributeAsInt("samples"));
      
      setSCoord(parseCoord(node.attribute("sCoord")));
      setTCoord(parseCoord(node.attribute("tCoord")));
      setRCoord(parseCoord(node.attribute("rCoord")));
      
      setMinFilter(parseFilter(node.attribute("min")));
      setMagFilter(parseFilter(node.attribute("mag")));
      setMipFilter(parseFilter(node.attribute("mip")));
      
      return true;
    }
    
    void setSamples(int samples)
    {
      samples = samples < 1 ? 1 : samples > SAMPLER_MAX_SAMPLES ? SAMPLER_MAX_SAMPLES : samples;
      flags = (flags & ~SAMPLER_SAMPLES_MASK) | ((samples - 1) & SAMPLER_SAMPLES_MASK);
    }
    int samples() const {return (flags & SAMPLER_SAMPLES_MASK) + 1;}
    
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
