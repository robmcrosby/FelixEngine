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
    STEREO_BINARY = 0x06,
    STEREO_ALL    = 0x07,
    STEREO_LEFT   = 0x02,
    STEREO_RIGHT  = 0x04,
  };
  
  class Window: public Resource
  {
  public:
    Window(): mMode(WINDOW_FULL_MONO) {}
    virtual ~Window() {}
    
    virtual bool setToXml(const XMLTree::Node &node);
    
    void setTitle(const std::string &title) {mTitle = title;}
    void setPosition(const ivec2 &pos) {mPosition = pos;}
    void setSize(const ivec2 &size) {mSize = size;}
    
    ivec2 position() const {return mPosition;}
    ivec2 size() const {return mSize;}
    
    void setMode(WINDOW_MODE mode) {mMode = mode;}
    WINDOW_MODE mode() const {return mMode;}
    
    int getStereoFlags() const
    {
      if (mMode == WINDOW_FULL_MONO)
        return STEREO_MONO;
      if (mMode == WINDOW_FULL_LEFT)
        return STEREO_LEFT | STEREO_MONO;
      if (mMode == WINDOW_FULL_RIGHT)
        return STEREO_RIGHT | STEREO_MONO;
      return STEREO_ALL;
    }
    
  protected:
    std::string mTitle;
    ivec2 mPosition, mSize;
    WINDOW_MODE mMode;
  };
  
  class Frame: public Resource
  {
  public:
    struct Buffer
    {
      COLOR_TYPE format;
      std::string name;
      Sampler sampler;
    };
    
  public:
    Frame(): mRefFrame(0) {}
    virtual ~Frame() {}
    
    virtual bool setToXml(const XMLTree::Node &node);
    
    void clearBuffers() {mBuffers.clear();}
    void addBuffer(COLOR_TYPE format = COLOR_RGBA, const std::string &name = "", const Sampler &sampler = Sampler())
    {
      Buffer buffer;
      buffer.format = format;
      buffer.name = name;
      buffer.sampler = sampler;
      addBuffer(buffer);
    }
    void addBuffer(const Buffer &buffer) {mBuffers.push_back(buffer);}
    
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
    std::list<Buffer> mBuffers;
  };
  
  class Shader: public Resource
  {
  public:
    Shader() {clearParts();}
    virtual ~Shader() {}
    
    virtual bool setToXml(const XMLTree::Node &node);
    
    void clearParts();
    void setSourceToPart(const std::string &src, SHADER_PART part);
    void setFileToPart(const std::string &file, SHADER_PART part);
    void setFunctionToPart(const std::string &func, SHADER_PART part);
    
    static SHADER_PART ParseShaderPart(const std::string &partStr);
    
  protected:
    std::string mParts[SHADER_COUNT];
    SHADER_TYPE mPartTypes[SHADER_COUNT];
  };
  
  class Mesh: public Resource
  {
  public:
    virtual ~Mesh() {}
    
    virtual bool setToXml(const XMLTree::Node &node);
    
    VertexBufferMap& getVertexBufferMap() {return mBufferMap;}
    void addVertexBuffer(const std::string &name, int components, int count, const float *data);
    void setIndexBuffer(int count, const int *data);
    void setPrimitiveType(VERTEX_PRIMITIVE type);
    
  protected:
    VertexBufferMap mBufferMap;
  };
  
  class Texture: public Resource
  {
  public:
    virtual ~Texture() {}
    
    virtual bool setToXml(const XMLTree::Node &node);
    
    void setImageFile(const std::string &file) {mFile = file;}
    std::string imageFile() const {return mFile;}
    
    void setSampler(const Sampler &sampler) {mSampler = sampler;}
    Sampler sampler() const {return mSampler;}
    
    void setSize(const ivec2 &size) {mSize = size;}
    ivec2 size() const {return mSize;}
    
  protected:
    std::string mFile;
    Sampler mSampler;
    ivec2 mSize;
  };
}

#endif /* GraphicResources_h */
