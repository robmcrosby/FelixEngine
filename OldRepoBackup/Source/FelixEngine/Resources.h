//
//  Resources.h
//  FelixEngine
//
//  Created by Robert Crosby on 3/22/16.
//  Copyright © 2016 Robert Crosby. All rights reserved.
//

#ifndef Resources_h
#define Resources_h

#include "Component.h"
#include "Scene.h"


namespace fx
{
  /**
   *
   */
  class Resources: public Component
  {
  public:
    Resources(Scene *scene): Component(scene) {}
    virtual ~Resources() {}
    
    virtual void setToXml(const XMLTree::Node &node)
    {
      Component::setToXml(node);
      for (const auto &subNode : node)
        add(*subNode);
    }
    
    void add(Buffer &buffer) {mBuffers.push_back(buffer);}
    void add(const XMLTree::Node &node)
    {
      if (node == "Frame")
        add(mScene->createFrame(node));
      else if (node == "Shader")
        add(mScene->createShader(node));
      else if (node == "Mesh")
        add(mScene->createMesh(node));
      else if (node == "Texture")
        add(mScene->createTexture(node));
    }
    void clear() {mBuffers.clear();}
    
  private:
    std::list<Buffer> mBuffers;
  };
}

#endif /* Resources_h */
