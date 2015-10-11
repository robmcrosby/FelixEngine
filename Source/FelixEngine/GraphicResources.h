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
  enum BUFFER_TYPE {
    BUFFER_RGBA,
    BUFFER_FLOAT,
    BUFFER_DEPTH32F,
    BUFFER_STENCIL8,
    BUFFER_NONE,
  };
  
  struct Window
  {
    virtual ~Window() {}
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool load() = 0;
    
    virtual void setTitle(const std::string &title) = 0;
    virtual void setPosition(const ivec2 &pos) = 0;
    virtual void setSize(const ivec2 &size) = 0;
  };
  
  struct Frame
  {
    virtual ~Frame() {}
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool load() = 0;
    
    virtual void addBuffer(BUFFER_TYPE type, const std::string &name, const Sampler &sampler) = 0;
    virtual void setSize(const ivec2 &size) = 0;
  };
  
  struct Shader
  {
    virtual ~Shader() {}
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool load() = 0;
    
    virtual void setVertexShaderSrc(const std::string &src) = 0;
    virtual void setFragmentShaderSrc(const std::string &src) = 0;
  };
  
  struct Mesh
  {
    virtual ~Mesh() {}
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool load() = 0;
    
    virtual bool addVertexBuffer(const std::string &name, int size, int count, const float *data) = 0;
    virtual bool setIndexBuffer(int count, const int *data) = 0;
    virtual void setPrimitiveType(VERTEX_PRIMITIVE type) = 0;
  };
  
  struct Texture
  {
    virtual ~Texture() {}
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool load() = 0;
  };
}

#endif /* GraphicResources_h */
