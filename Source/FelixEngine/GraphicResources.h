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
#include "Sampler.h"


namespace fx
{
  class GraphicSystem;
  
  enum BUFFER_TYPE {
    BUFFER_RGBA,
    BUFFER_FLOAT,
    BUFFER_DEPTH32F,
    BUFFER_STENCIL8,
    BUFFER_NONE,
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
  
  struct Window: GraphicResource
  {
    virtual ~Window() {}
    virtual bool setToXml(const XMLTree::Node *node);
    
    virtual void setTitle(const std::string &title) = 0;
    virtual void setPosition(const ivec2 &pos) = 0;
    virtual void setSize(const ivec2 &size) = 0;
  };
  
  struct Frame: GraphicResource
  {
    virtual ~Frame() {}
    virtual bool setToXml(const XMLTree::Node *node);
    
    virtual void addBuffer(BUFFER_TYPE type, const std::string &name, const Sampler &sampler) = 0;
    virtual void setSize(const ivec2 &size) = 0;
  };
  
  struct Shader: GraphicResource
  {
    virtual ~Shader() {}
    virtual bool setToXml(const XMLTree::Node *node);
    
    virtual void setVertexShaderSrc(const std::string &src) = 0;
    virtual void setFragmentShaderSrc(const std::string &src) = 0;
  };
  
  class Mesh: public GraphicResource
  {
  public:
    virtual ~Mesh() {}
    virtual bool setToXml(const XMLTree::Node *node);
    
    virtual void setToReload() = 0;
    virtual void addVertexBuffer(const std::string &name, int components, int count, const float *data) = 0;
    virtual void setIndexBuffer(int count, const int *data) = 0;
    virtual void setPrimitiveType(VERTEX_PRIMITIVE type) = 0;
    
  protected:
    VertexBufferMap mBufferMap;
  };
  
  struct Texture: GraphicResource
  {
    virtual ~Texture() {}
    virtual bool setToXml(const XMLTree::Node *node);
  };
  
  class Material: public GraphicResource
  {
  public:
    Material(GraphicSystem *system);
    virtual ~Material();
    
    virtual bool setToXml(const XMLTree::Node *node);
  private:
    GraphicSystem *mSystem;
  };
  
  class View: public GraphicResource
  {
  public:
    View(GraphicSystem *system);
    virtual ~View();
    
    virtual bool setToXml(const XMLTree::Node *node);
  private:
    GraphicSystem *mSystem;
  };
}

#endif /* GraphicResources_h */
