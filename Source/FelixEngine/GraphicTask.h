//
//  GraphicTask.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/11/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GraphicTask_h
#define GraphicTask_h

#include "Color.h"
#include "XMLTree.h"

namespace fx
{
  class Frame;
  class Shader;
  class Mesh;
  class TextureMap;
  
  class InternalUniformMap;
  
  
  enum COLOR_ATTACHMENTS
  {
    COLOR_ATTACHMENT_0,
    COLOR_ATTACHMENT_1,
    COLOR_ATTACHMENT_2,
    COLOR_ATTACHMENT_3,
    COLOR_ATTACHMENT_MAX,
  };
  
  enum CLEAR_FLAGS
  {
    CLEAR_COLOR    = 0x01,
    CLEAR_DEPTH    = 0x02,
    CLEAR_STENCEIL = 0x04,
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
  
  enum BLEND_FUNC_FLAGS
  {
    BLEND_FUNC_MASK       = 0xff00,
    BLEND_FUNC_COLOR_MASK = 0x0f00,
    BLEND_FUNC_ALPHA_MASK = 0xf000,
    
    BLEND_FUNC_ADD          = 0x0100,
    BLEND_FUNC_SUBTRACT     = 0x0200,
    BLEND_FUNC_REV_SUBTRACT = 0x0300,
    BLEND_FUNC_MIN          = 0x0400,
    BLEND_FUNC_MAX          = 0x0500,
    
    BLEND_FUNC_ADD_ALPHA          = 0x1000,
    BLEND_FUNC_SUBTRACT_ALPHA     = 0x2000,
    BLEND_FUNC_REV_SUBTRACT_ALPHA = 0x3000,
    BLEND_FUNC_MIN_ALPHA          = 0x4000,
    BLEND_FUNC_MAX_ALPHA          = 0x5000,
  };
  
  /**
   * 0000 0000 0000 0000 #### #### 0000 0000
   */
  enum BLEND_INPUT_FLAGS {
    BLEND_INPUT_MASK        = 0xffff0000,
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
    
    BLEND_INPUT_SRC_COLOR_SHIFT  = 12,
    BLEND_INPUT_DEST_COLOR_SHIFT = 16,
    BLEND_INPUT_SRC_ALPHA_SHIFT  = 20,
    BLEND_INPUT_DEST_ALPHA_SHIFT = 24,
  };
  
  
  
  struct ClearState
  {
    ClearState() {reset();}
    ClearState(const ClearState &other) {*this = other;}
    ClearState(const XMLTree::Node &node)
    {
      reset();
      setToXml(node);
    }
    
    ClearState& operator=(const ClearState &other)
    {
      flags = other.flags;
      depth = other.depth;
      stenceil = other.stenceil;
      for (int i = 0; i < COLOR_ATTACHMENT_MAX; ++i)
        colors[i] = other.colors[i];
      return *this;
    }
    
    void setClearColor(const RGBAf &color, unsigned int index = 0)
    {
      if (index < COLOR_ATTACHMENT_MAX)
      {
        colors[index] = color;
        flags |= CLEAR_COLOR;
      }
    }
    void setClearDepth(float value = 0.0f)
    {
      depth = value;
      flags |= CLEAR_DEPTH;
    }
    void setClearStencil(int value = 0)
    {
      stenceil = value;
      flags |= CLEAR_STENCEIL;
    }
    void reset()
    {
      flags = 0;
      depth = 0.0f;
      stenceil = 0;
      for (int i = 0; i < COLOR_ATTACHMENT_MAX; ++i)
        colors[i] = RGBAf(0.0f, 0.0f, 0.0f, 1.0f);
    }
    bool setToXml(const XMLTree::Node &node)
    {
      bool success = true;
      
      char buffer[16];
      for (int i = 0; i < COLOR_ATTACHMENT_MAX; ++i)
      {
        sprintf(buffer, "Color%i", i);
        if (node.hasSubNode(buffer))
        {
          success &= colors[i].parse(node.subContents(buffer));
          flags |= CLEAR_COLOR;
        }
      }
      
      if (node.hasAttribute("depth"))
        setClearDepth(node.attributeAsFloat("depth"));
      
      if (node.hasAttribute("stencil"))
        setClearStencil(node.attributeAsInt("stencil"));
      
      return success;
    }
    
    int   flags;
    float depth;
    int   stenceil;
    RGBAf colors[COLOR_ATTACHMENT_MAX];
  };
  
  
  
  struct DepthState
  {
    DepthState(): flags(0) {}
    void setWriting(bool writing = true)
    {
      if (writing)
        flags |= DEPTH_ENABLE_WRITING;
      else
        flags &= ~DEPTH_ENABLE_WRITING;
    }
    void setTesting(bool testing = true)
    {
      if (testing)
        flags |= DEPTH_ENABLE_TESTING;
      else
        flags &= ~DEPTH_ENABLE_TESTING;
    }
    void setTestFunction(int function)
    {
      flags = (function & DEPTH_TEST_MASK) | (flags & DEPTH_ENABLE_MASK);
    }
    
    bool setToXml(const XMLTree::Node &node)
    {
      setWriting(node.attributeAsBoolean("write"));
      
      if (node.hasAttribute("function"))
      {
        setTesting(true);
        
        std::string str = node.attribute("function");
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        
        if (str == "less_equal")
          setTestFunction(DEPTH_TEST_LESS_EQ);
        else if (str == "greater_equal")
          setTestFunction(DEPTH_TEST_GREATER_EQ);
        else if (str == "less")
          setTestFunction(DEPTH_TEST_LESS);
        else if (str == "greater")
          setTestFunction(DEPTH_TEST_GREATER);
        else if (str == "equal")
          setTestFunction(DEPTH_TEST_EQUAL);
        else if (str == "never")
          setTestFunction(DEPTH_TEST_NEVER);
        else
          setTestFunction(DEPTH_TEST_ALWAYS);
      }
      else
      {
        setTesting(false);
        setTestFunction(DEPTH_TEST_ALWAYS);
      }
      
      return true;
    }
    
    bool isWritingEnabled() const {return flags & DEPTH_ENABLE_WRITING;}
    bool isTestingEnabled() const {return flags & DEPTH_ENABLE_TESTING;}
    int  getTestFunction()  const {return flags & DEPTH_TEST_MASK;}
    
    int flags;
  };
  
  
  
  struct BlendState
  {
    BlendState(): flags(0) {}
    
    bool setToXml(const XMLTree::Node &node)
    {
      setFunction(BLEND_FUNC_ADD);
      return true;
    }
    
    void setFunction(int func) {flags = (flags & ~BLEND_FUNC_MASK) | (func & BLEND_FUNC_MASK);}
    void setColorFunction(int func) {flags = (flags & ~BLEND_FUNC_COLOR_MASK) | (func & BLEND_FUNC_COLOR_MASK);}
    void setAlphaFunction(int func) {flags = (flags & ~BLEND_FUNC_ALPHA_MASK) | (func & BLEND_FUNC_ALPHA_MASK);}
    
    void setInput(int input, int shift)
    {
      int mask = BLEND_INPUT_MASK << shift;
      flags = (flags & ~mask) | ((input << shift) & mask);
    }
    void setInputColorSrc(int input)  {setInput(input, BLEND_INPUT_SRC_COLOR_SHIFT);}
    void setInputColorDest(int input) {setInput(input, BLEND_INPUT_DEST_COLOR_SHIFT);}
    void setInputAlphaSrc(int input)  {setInput(input, BLEND_INPUT_SRC_ALPHA_SHIFT);}
    void setInputAlphaDest(int input) {setInput(input, BLEND_INPUT_DEST_ALPHA_SHIFT);}
    
    int colorFunction() const {return flags & BLEND_FUNC_COLOR_MASK;}
    int alphaFunction() const {return flags & BLEND_FUNC_ALPHA_MASK;}
    
    int inputColorSrc()  const {return (flags >> BLEND_INPUT_SRC_COLOR_SHIFT) & BLEND_INPUT_MASK;}
    int inputColorDest() const {return (flags >> BLEND_INPUT_DEST_COLOR_SHIFT) & BLEND_INPUT_MASK;}
    int inputAlphaSrc()  const {return (flags >> BLEND_INPUT_SRC_ALPHA_SHIFT) & BLEND_INPUT_MASK;}
    int inputAlphaDest() const {return (flags >> BLEND_INPUT_DEST_ALPHA_SHIFT) & BLEND_INPUT_MASK;}
    
    bool isBlendingEnabled() const {return flags & BLEND_FUNC_MASK;}
    bool isBlendSeperate() const {return flags & BLEND_FUNC_ALPHA_MASK;}
    
    int   flags;
    RGBAf color;
  };
  
  
  
  struct GraphicTask
  {
    GraphicTask(): frame(0), shader(0), mesh(0), localUniforms(0), materialUniforms(0),
    textureMap(0), layer(0), subMesh(0), viewIndex(0), instances(1) {}
    
    bool isViewTask()  const {return frame && !mesh && !shader && viewIndex > 0;}
    bool isDrawTask()  const {return mesh && shader && instances > 0;}
    bool isClearTask() const {return clearState.flags;}
    
    void setClearColor(const RGBAf &color, unsigned int index = 0) {clearState.setClearColor(color, index);}
    void setClearDepth(float depth = 0.0f) {clearState.setClearDepth(depth);}
    void setClearStencil(int value = 0)    {clearState.setClearStencil(value);}
    
    bool operator<(const GraphicTask &that)  const {return layer < that.layer;}
    bool operator==(const GraphicTask &that) const {return layer == that.layer;}
    
    const Frame  *frame;
    const Shader *shader;
    const Mesh   *mesh;
    
    const InternalUniformMap *localUniforms;
    const InternalUniformMap *materialUniforms;
    
    const TextureMap *textureMap;
    
    int layer;
    int subMesh;
    int viewIndex;
    int instances;
    
    ClearState clearState;
    DepthState depthState;
    BlendState blendState;
  };
}

#endif /* GraphicTask_h */
