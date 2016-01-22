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
#include "BufferMap.h"
#include "GraphicResources.h"


namespace fx
{
  class Frame;
  class Shader;
  class Mesh;
  class TextureMap;
  
  class InternalUniformMap;
  
  
  enum GRAPHIC_TASK_TYPE
  {
    GRAPHIC_TASK_UPLOAD,   /**< Upload the contents of the buffers to the GPU resources */
    GRAPHIC_TASK_UNLOAD,   /**< Unload the GPU resources associated with the buffers */
    GRAPHIC_TASK_DOWNLOAD, /**< Download the GPU resources to the buffers */
    GRAPHIC_TASK_PASS,     /**< Runs a group of Draw/Compute tasks associated with a numbered pass */
    GRAPHIC_TASK_DRAW,     /**< Perform a Draw operation */
    GRAPHIC_TASK_COMPUTE,  /**< Perform a Compute operation */
    GRAPHIC_TASK_EMPTY,    /**< Does nothing */
  };
  
  enum BUFFER_SLOTS
  {
    BUFFER_SLOT_SHADER   = 0,
    BUFFER_SLOT_MESH     = 1,
    BUFFER_SLOT_UNIFORMS = 2,
    BUFFER_SLOT_TARGETS  = 3,
    BUFFER_SLOTS_SIZE    = 4,
  };
  
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
  
  enum CULL_MODE
  {
    CULL_NONE,
    CULL_FRONT,
    CULL_BACK,
  };
  
  /**
   * 0000 0000 0000 0000 0000 0000 #### ####
   */
  enum BLEND_EQ_FLAGS
  {
    BLEND_EQ_MASK         = 0xff,
    BLEND_EQ_COMP_MASK    = 0xf,
    
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
  enum BLEND_INPUT_FLAGS {
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
    void setFunction(int function)
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
          setFunction(DEPTH_TEST_LESS_EQ);
        else if (str == "greater_equal")
          setFunction(DEPTH_TEST_GREATER_EQ);
        else if (str == "less")
          setFunction(DEPTH_TEST_LESS);
        else if (str == "greater")
          setFunction(DEPTH_TEST_GREATER);
        else if (str == "equal")
          setFunction(DEPTH_TEST_EQUAL);
        else if (str == "never")
          setFunction(DEPTH_TEST_NEVER);
        else
          setFunction(DEPTH_TEST_ALWAYS);
      }
      else
      {
        setTesting(false);
        setFunction(DEPTH_TEST_ALWAYS);
      }
      
      return true;
    }
    
    bool enabled() const {return writingEnabled() || testingEnabled();}
    bool writingEnabled() const {return flags & DEPTH_ENABLE_WRITING;}
    bool testingEnabled() const {return flags & DEPTH_ENABLE_TESTING;}
    int  function()  const {return flags & DEPTH_TEST_MASK;}
    
    int flags;
  };
  
  
  
  struct BlendState
  {
    BlendState(int f = 0): flags(f) {}
    
    void setToXml(const XMLTree::Node &node)
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
        color = RGBAf(node.attribute("color"));
      else if (node.hasSubNode("Color"))
        color = RGBAf(node.subContents("Color"));
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
    }
    
    void setEquation(int func, int shift = BLEND_EQ_COLOR_SHIFT) {flags = (flags & ~(BLEND_EQ_COMP_MASK << shift)) | ((func << shift) & (BLEND_EQ_COMP_MASK << shift));}
    void setEquationAlpha(int func) {setEquation(func, BLEND_EQ_ALPHA_SHIFT);}
    
    void setEquation(const std::string &str, int shift = BLEND_EQ_COLOR_SHIFT) {setEquation(GetEquationFlag(str), shift);}
    void setEquationAlpha(const std::string &str) {setEquation(str, BLEND_EQ_ALPHA_SHIFT);}
    
    void setInput(int input, int shift)
    {
      int mask = BLEND_INPUT_MASK << shift;
      flags = (flags & ~mask) | ((input << shift) & mask);
    }
    void setSrc(int input) {setInput(input, BLEND_INPUT_SRC_COLOR_SHIFT);}
    void setDst(int input) {setInput(input, BLEND_INPUT_DEST_COLOR_SHIFT);}
    void setSrcAlpha(int input) {setInput(input, BLEND_INPUT_SRC_ALPHA_SHIFT);}
    void setDstAlpha(int input) {setInput(input, BLEND_INPUT_DEST_ALPHA_SHIFT);}
    
    void setSrc(const std::string &str) {setSrc(GetInputFlag(str));}
    void setDst(const std::string &str) {setDst(GetInputFlag(str));}
    void setSrcAlpha(const std::string &str) {setSrcAlpha(GetInputFlag(str));}
    void setDstAlpha(const std::string &str) {setDstAlpha(GetInputFlag(str));}
    
    int equation() const {return (flags >> BLEND_EQ_COLOR_SHIFT) & BLEND_EQ_COMP_MASK;}
    int equationAlpha() const {return (flags >> BLEND_EQ_ALPHA_SHIFT) & BLEND_EQ_COMP_MASK;}
    
    int src() const {return (flags >> BLEND_INPUT_SRC_COLOR_SHIFT) & BLEND_INPUT_MASK;}
    int dst() const {return (flags >> BLEND_INPUT_DEST_COLOR_SHIFT) & BLEND_INPUT_MASK;}
    int srcAlpha() const {return (flags >> BLEND_INPUT_SRC_ALPHA_SHIFT) & BLEND_INPUT_MASK;}
    int dstAlpha() const {return (flags >> BLEND_INPUT_DEST_ALPHA_SHIFT) & BLEND_INPUT_MASK;}
    
    bool enabled() const {return (flags >> BLEND_EQ_COLOR_SHIFT) & BLEND_EQ_MASK;}
    bool seperate() const {return (flags >> BLEND_EQ_ALPHA_SHIFT) & BLEND_EQ_COMP_MASK;}
    
    static int GetInputFlag(const std::string &str)
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
    
    static int GetEquationFlag(const std::string &str)
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
    
    int   flags;
    RGBAf color;
  };
  
  
  struct DrawState
  {
    DrawState(): submesh(0), instances(1), stereo(STEREO_ALL), cullMode(CULL_NONE) {}
    
    void setToXml(const XMLTree::Node &node)
    {
      submesh = node.hasAttribute("submesh") ? node.attributeAsInt("submesh") : 0;
      instances = node.hasAttribute("instances") ? node.attributeAsInt("instances") : 1;
      stereo = GetStereoFlags(node.attribute("stereo"));
      
      if (node.hasSubNode("ClearState"))
        clearState.setToXml(*node.subNode("ClearState"));
      else
        clearState = ClearState();
      
      if (node.hasSubNode("DepthState"))
        depthState.setToXml(*node.subNode("DepthState"));
      else
        depthState = DepthState();
      
      if (node.hasSubNode("BlendState"))
        blendState.setToXml(*node.subNode("BlendState"));
      else
        blendState = BlendState();
    }
    
    static int GetStereoFlags(const std::string &flags)
    {
      if (flags == "left")
        return STEREO_LEFT;
      if (flags == "right")
        return STEREO_RIGHT;
      if (flags == "binary")
        return STEREO_BINARY;
      if (flags == "mono")
        return STEREO_MONO;
      return STEREO_ALL;
    }
    
    int submesh;
    int instances;
    int stereo;
    
    ClearState clearState;
    DepthState depthState;
    BlendState blendState;
    
    CULL_MODE cullMode;
  };
  
  struct GraphicTask
  {
    GraphicTask(GRAPHIC_TASK_TYPE type = GRAPHIC_TASK_EMPTY): type(type), layer(0), pass(0), textureMap(0) {clearBufferSlots();}
    void clearBufferSlots()
    {
      for (int i = 0; i < BUFFER_SLOTS_SIZE; ++i)
        bufferSlots[i] = nullptr;
    }
    
    bool operator<(const GraphicTask &other) const {return layer < other.layer;}
    bool operator==(const GraphicTask &other) const {return layer == other.layer;}
    bool operator!=(const GraphicTask &other) const {return layer != other.layer;}
    
    GRAPHIC_TASK_TYPE type;
    DrawState drawState;
    
    int layer;
    int pass;
    
    BufferMap *bufferSlots[BUFFER_SLOTS_SIZE];
    TextureMap *textureMap;
  };
  
  
  
//  struct GraphicTask
//  {
//    GraphicTask(): frame(0), shader(0), mesh(0), localUniforms(0), materialUniforms(0),
//    textureMap(0), layer(0), subMesh(0), pass(0), instances(1), stereo(STEREO_ALL), cullMode(CULL_NONE) {}
//    
//    bool isViewTask()  const {return frame && !mesh && !shader && pass > 0;}
//    bool isDrawTask()  const {return mesh && shader && instances > 0;}
//    bool isClearTask() const {return clearState.flags;}
//    
//    void setClearColor(const RGBAf &color, unsigned int index = 0) {clearState.setClearColor(color, index);}
//    void setClearDepth(float depth = 0.0f) {clearState.setClearDepth(depth);}
//    void setClearStencil(int value = 0)    {clearState.setClearStencil(value);}
//    
//    bool operator<(const GraphicTask &that)  const {return layer < that.layer;}
//    bool operator==(const GraphicTask &that) const {return layer == that.layer;}
//    
//    const Frame  *frame;
//    const Shader *shader;
//    const Mesh   *mesh;
//    
//    const InternalUniformMap *localUniforms;
//    const InternalUniformMap *materialUniforms;
//    
//    const TextureMap *textureMap;
//    
//    int layer;
//    int pass;
//    int subMesh;
//    int instances;
//    int stereo;
//    
//    ClearState clearState;
//    DepthState depthState;
//    BlendState blendState;
//    
//    CULL_MODE cullMode;
//  };
}

#endif /* GraphicTask_h */
