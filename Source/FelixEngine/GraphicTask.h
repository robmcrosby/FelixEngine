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
  class UniformMap;
  class TextureMap;
  
  
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
    ClearState(const XMLTree::Node &node)
    {
      reset();
      setToXml(node);
    }
    void setClearColor(const RGBAf &color, unsigned int index = 0)
    {
      if (index < COLOR_ATTACHMENT_MAX)
      {
        mClearColors[index] = color;
        mClearFlags |= CLEAR_COLOR;
      }
    }
    void setClearDepth(float depth = 0.0f)
    {
      mClearDepth = depth;
      mClearFlags |= CLEAR_DEPTH;
    }
    void setClearStencil(int value = 0)
    {
      mClearStenceil = value;
      mClearFlags |= CLEAR_STENCEIL;
    }
    void reset()
    {
      mClearFlags = 0;
      mClearDepth = 0.0f;
      mClearStenceil = 0;
      for (int i = 0; i < COLOR_ATTACHMENT_MAX; ++i)
        mClearColors[i] = RGBAf(0.0f, 0.0f, 0.0f, 1.0f);
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
          success &= mClearColors[i].parse(node.subContents(buffer));
          mClearFlags |= CLEAR_COLOR;
        }
      }
      
      if (node.hasAttribute("depth"))
        setClearDepth(node.attributeAsFloat("depth"));
      
      if (node.hasAttribute("stencil"))
        setClearStencil(node.attributeAsInt("stencil"));
      
      return success;
    }
    
    int   mClearFlags;
    float mClearDepth;
    int   mClearStenceil;
    RGBAf mClearColors[COLOR_ATTACHMENT_MAX];
  };
  
  
  
  struct DepthState
  {
    DepthState(): mDepthFlags(0) {}
    void setWriting(bool writing = true)
    {
      if (writing)
        mDepthFlags |= DEPTH_ENABLE_WRITING;
      else
        mDepthFlags &= ~DEPTH_ENABLE_WRITING;
    }
    void setTesting(bool testing = true)
    {
      if (testing)
        mDepthFlags |= DEPTH_ENABLE_TESTING;
      else
        mDepthFlags &= ~DEPTH_ENABLE_TESTING;
    }
    void setTestFunction(int function)
    {
      mDepthFlags = (function & DEPTH_TEST_MASK) | (mDepthFlags & DEPTH_ENABLE_MASK);
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
    
    bool isWritingEnabled() const {return mDepthFlags & DEPTH_ENABLE_WRITING;}
    bool isTestingEnabled() const {return mDepthFlags & DEPTH_ENABLE_TESTING;}
    int  getTestFunction()  const {return mDepthFlags & DEPTH_TEST_MASK;}
    
    int mDepthFlags;
  };
  
  
  
  struct BlendState
  {
    BlendState(): mBlendFlags(0) {}
    
    bool setToXml(const XMLTree::Node &node)
    {
      setFunction(BLEND_FUNC_ADD);
      return true;
    }
    
    void setFunction(int func) {mBlendFlags = (mBlendFlags & ~BLEND_FUNC_MASK) | (func & BLEND_FUNC_MASK);}
    void setColorFunction(int func) {mBlendFlags = (mBlendFlags & ~BLEND_FUNC_COLOR_MASK) | (func & BLEND_FUNC_COLOR_MASK);}
    void setAlphaFunction(int func) {mBlendFlags = (mBlendFlags & ~BLEND_FUNC_ALPHA_MASK) | (func & BLEND_FUNC_ALPHA_MASK);}
    
    void setInput(int input, int shift)
    {
      int mask = BLEND_INPUT_MASK << shift;
      mBlendFlags = (mBlendFlags & ~mask) | ((input << shift) & mask);
    }
    void setInputColorSrc(int input)  {setInput(input, BLEND_INPUT_SRC_COLOR_SHIFT);}
    void setInputColorDest(int input) {setInput(input, BLEND_INPUT_DEST_COLOR_SHIFT);}
    void setInputAlphaSrc(int input)  {setInput(input, BLEND_INPUT_SRC_ALPHA_SHIFT);}
    void setInputAlphaDest(int input) {setInput(input, BLEND_INPUT_DEST_ALPHA_SHIFT);}
    
    int colorFunction() const {return mBlendFlags & BLEND_FUNC_COLOR_MASK;}
    int alphaFunction() const {return mBlendFlags & BLEND_FUNC_ALPHA_MASK;}
    
    int inputColorSrc()  const {return (mBlendFlags >> BLEND_INPUT_SRC_COLOR_SHIFT) & BLEND_INPUT_MASK;}
    int inputColorDest() const {return (mBlendFlags >> BLEND_INPUT_DEST_COLOR_SHIFT) & BLEND_INPUT_MASK;}
    int inputAlphaSrc()  const {return (mBlendFlags >> BLEND_INPUT_SRC_ALPHA_SHIFT) & BLEND_INPUT_MASK;}
    int inputAlphaDest() const {return (mBlendFlags >> BLEND_INPUT_DEST_ALPHA_SHIFT) & BLEND_INPUT_MASK;}
    
    bool isBlendingEnabled() const {return mBlendFlags & BLEND_FUNC_MASK;}
    bool isBlendSeperate() const {return mBlendFlags & BLEND_FUNC_ALPHA_MASK;}
    
    int   mBlendFlags;
    RGBAf mBlendColor;
  };
  
  
  
  struct GraphicTask
  {
    GraphicTask(): mFrame(0), mShader(0), mMesh(0), mLocalUniforms(0), mViewUniforms(0), mMaterialUniforms(0),
    mTextureMap(0), mLayer(0), mSubMesh(0), mViewIndex(0), mInstances(1) {}
    
    bool isViewTask()  const {return mFrame && !mMesh && !mShader && mViewIndex > 0;}
    bool isDrawTask()  const {return mFrame && mMesh && mShader && mInstances > 0;}
    bool isClearTask() const {return mFrame && mClearState.mClearFlags;}
    
    void setClearColor(const RGBAf &color, unsigned int index = 0) {mClearState.setClearColor(color, index);}
    void setClearDepth(float depth = 0.0f) {mClearState.setClearDepth(depth);}
    void setClearStencil(int value = 0)    {mClearState.setClearStencil(value);}
    
    bool operator<(const GraphicTask &that)  const {return mLayer < that.mLayer;}
    bool operator==(const GraphicTask &that) const {return mLayer == that.mLayer;}
    
    const Frame  *mFrame;
    const Shader *mShader;
    const Mesh   *mMesh;
    
    const UniformMap *mLocalUniforms;
    const UniformMap *mViewUniforms;
    const UniformMap *mMaterialUniforms;
    
    const TextureMap *mTextureMap;
    
    int mLayer;
    int mSubMesh;
    int mViewIndex;
    int mInstances;
    
    ClearState mClearState;
    DepthState mDepthState;
    BlendState mBlendState;
  };
}

#endif /* GraphicTask_h */
