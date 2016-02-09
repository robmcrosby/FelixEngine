//
//  GraphicResources.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GraphicResources_h
#define GraphicResources_h

#include "Resource.h"
#include "VertexBufferMap.h"
#include "XMLTree.h"
#include "Vector.h"
#include "Color.h"
#include "Sampler.h"

#define MAIN_WINDOW "MainWindow"


namespace fx
{
  class GraphicSystem;
  
  enum SHADER_PART {
    SHADER_VERTEX = 0,
    SHADER_FRAGMENT,
    SHADER_COUNT,
  };
  
  enum SHADER_TYPE {
    SHADER_EMPTY,
    SHADER_SOURCE,
    SHADER_FILE,
    SHADER_FUNCTION,
  };
  
  enum WINDOW_MODE {
    WINDOW_FULL_MONO,
    WINDOW_FULL_LEFT,
    WINDOW_FULL_RIGHT,
    WINDOW_LEFT_RIGHT,
    WINDOW_RIGHT_LEFT,
    WINDOW_LEFT_OVER_RIGHT,
    WINDOW_RIGHT_OVER_LEFT,
  };
  
  enum STEREO_FLAGS
  {
    STEREO_MONO   = 0x01,
    STEREO_LEFT   = 0x02,
    STEREO_RIGHT  = 0x04,
    STEREO_BINARY = 0x06,
    STEREO_ALL    = 0x07,
  };
  
  class Window: public Resource
  {
  public:
    Window(): mMode(WINDOW_FULL_MONO), mScale(1.0f) {}
    virtual ~Window() {}
    
    bool setToXml(const XMLTree::Node &node);
    
    void setTitle(const std::string &title) {mTitle = title;}
    void setPosition(const ivec2 &pos) {mPosition = pos;}
    void setSize(const ivec2 &size) {mSize = size;}
    void setScale(float scale) {mScale = scale;}
    
    ivec2 position() const {return mPosition;}
    ivec2 size() const {return mSize;}
    ivec2 frameSize()
    {
      ivec2 size = mSize;
      if (mMode == WINDOW_LEFT_OVER_RIGHT || mMode == WINDOW_RIGHT_OVER_LEFT)
        size.y /= 2;
      else if (mMode == WINDOW_LEFT_RIGHT || mMode == WINDOW_RIGHT_LEFT)
        size.x /= 2;
      return size;
    }
    
    float scale() const {return mScale;}
    
    void setMode(WINDOW_MODE mode) {mMode = mode;}
    WINDOW_MODE mode() const {return mMode;}
    
    int getStereoFlags() const
    {
      if (mMode == WINDOW_FULL_MONO)
        return STEREO_MONO;
      if (mMode == WINDOW_FULL_LEFT)
        return STEREO_LEFT;
      if (mMode == WINDOW_FULL_RIGHT)
        return STEREO_RIGHT;
      return STEREO_BINARY;
    }
    
  protected:
    std::string mTitle;
    ivec2 mPosition, mSize;
    float mScale;
    WINDOW_MODE mMode;
  };
  
  class Frame: public Resource
  {
  public:
    Frame(): mRefFrame(0) {}
    virtual ~Frame() {}
    
    //virtual bool setToXml(const XMLTree::Node &node);
    
    void setSize(const ivec2 &size) {mSize = size;}
    ivec2 size() const {return mSize;}
    
    void setScale(const vec2 &scale) {mScale = scale;}
    vec2 scale() const {return mScale;}
    
    void setRefrenceFrame(const std::string &name);
    void setRefrenceFrame(Frame *frame) {mRefFrame = frame != this ? frame : nullptr;}
    Frame* refrenceFrame() const {return mRefFrame;}
    
  protected:
    ivec2 mSize;
    vec2 mScale;
    Frame *mRefFrame;
  };
  
  class Shader: public Resource
  {
  public:
    Shader() {}
    virtual ~Shader() {}
    
    static SHADER_PART ParseShaderPart(const std::string &partStr);
  };
  
  class Mesh: public Resource
  {
  public:
    virtual ~Mesh() {}
  };
  
  class Texture: public Resource
  {
  public:
    virtual ~Texture() {}
    ivec2 size() const {return mSize;}
    
  protected:
    ivec2 mSize;
  };
}

#endif /* GraphicResources_h */
