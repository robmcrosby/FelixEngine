//
//  GraphicResources.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GraphicResources_h
#define GraphicResources_h

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
  
  class GraphicResource
  {
  public:
    GraphicResource(): mLoaded(0) {}
    virtual ~GraphicResource() {}
    
    virtual bool setToXml(const XMLTree::Node *node) = 0;
    virtual void reload() {mLoaded = false;}
    
    bool loaded() const {return mLoaded;}
    
  protected:
    bool mLoaded;
  };
  
  class Window: public GraphicResource
  {
  public:
    virtual ~Window() {}
    virtual bool setToXml(const XMLTree::Node *node);
    
    virtual void setTitle(const std::string &title) = 0;
    virtual void setPosition(const ivec2 &pos) = 0;
    virtual void setSize(const ivec2 &size) = 0;
    
    ivec2 position() const {return mPosition;}
    ivec2 size() const {return mSize;}
    
  protected:
    ivec2 mPosition, mSize;
  };
  
  class Frame: public GraphicResource
  {
  public:
    virtual ~Frame() {}
    
    bool setToXml(const XMLTree::Node &node) {return setToXml(&node);}
    virtual bool setToXml(const XMLTree::Node *node);
    
    virtual void addBuffer(COLOR_TYPE type, const std::string &name, const Sampler &sampler) = 0;
    virtual void setSize(const ivec2 &size) = 0;
    
    ivec2 size() const {return mSize;}
    
  protected:
    ivec2 mSize;
  };
  
  class Shader: public GraphicResource
  {
  public:
    Shader() {clearParts();}
    virtual ~Shader() {}
    
    bool setToXml(const XMLTree::Node &node) {return setToXml(&node);}
    virtual bool setToXml(const XMLTree::Node *node);
    
    void clearParts();
    void setSourceToPart(const std::string &src, SHADER_PART part);
    void setFileToPart(const std::string &file, SHADER_PART part);
    void setFunctionToPart(const std::string &func, SHADER_PART part);
    
    static SHADER_PART ParseShaderPart(const std::string &partStr);
    
  protected:
    std::string mParts[SHADER_COUNT];
    SHADER_TYPE mPartTypes[SHADER_COUNT];
  };
  
  class Mesh: public GraphicResource
  {
  public:
    virtual ~Mesh() {}
    
    bool setToXml(const XMLTree::Node &node) {return setToXml(&node);}
    virtual bool setToXml(const XMLTree::Node *node);
    
    VertexBufferMap& getVertexBufferMap() {return mBufferMap;}
    void addVertexBuffer(const std::string &name, int components, int count, const float *data);
    void setIndexBuffer(int count, const int *data);
    void setPrimitiveType(VERTEX_PRIMITIVE type);
    
  protected:
    VertexBufferMap mBufferMap;
  };
  
  struct Texture: GraphicResource
  {
    virtual ~Texture() {}
    virtual bool setToXml(const XMLTree::Node *node);
  };
}

#endif /* GraphicResources_h */
