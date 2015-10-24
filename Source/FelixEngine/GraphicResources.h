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
  
  class Window: public Resource
  {
  public:
    virtual ~Window() {}
    
    bool setToXml(const XMLTree::Node *node) {return node && setToXml(*node);}
    bool setToXml(const XMLTree::Node &node);
    
    void setTitle(const std::string &title) {mTitle = title;}
    void setPosition(const ivec2 &pos) {mPosition = pos;}
    void setSize(const ivec2 &size) {mSize = size;}
    
    ivec2 position() const {return mPosition;}
    ivec2 size() const {return mSize;}
    
  protected:
    std::string mTitle;
    ivec2 mPosition, mSize;
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
    virtual ~Frame() {}
    
    bool setToXml(const XMLTree::Node *node) {return node && setToXml(*node);}
    bool setToXml(const XMLTree::Node &node);
    
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
    
    void setRefrenceFrame(const std::string &name) {mRefFrame = name;}
    std::string refrenceFrame() const {return mRefFrame;}
    
  protected:
    ivec2 mSize;
    vec2 mScale;
    std::string mRefFrame;
    std::list<Buffer> mBuffers;
  };
  
  class Shader: public Resource
  {
  public:
    Shader() {clearParts();}
    virtual ~Shader() {}
    
    bool setToXml(const XMLTree::Node *node) {return node && setToXml(*node);}
    bool setToXml(const XMLTree::Node &node);
    
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
    
    bool setToXml(const XMLTree::Node *node) {return node && setToXml(*node);}
    bool setToXml(const XMLTree::Node &node);
    
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
    
    bool setToXml(const XMLTree::Node *node) {return node && setToXml(*node);}
    bool setToXml(const XMLTree::Node &node);
    
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
