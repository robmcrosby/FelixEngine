//
//  Scene.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/2/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Scene_h
#define Scene_h


#include "Component.h"
#include "XMLTree.h"

#include "GraphicSystem.h"
#include "FileSystem.h"

#include <istream>
#include <list>

namespace fx
{
  class Buffer;
  class Material;
  
  class Scene: public Component
  {
  public:
    Scene();
    ~Scene();
    
    virtual void handle(const Event &event);
    virtual void setToXml(const XMLTree::Node &node);
    bool init();
    
    Buffer& getFrameBuffer(const std::string &name);
    Buffer& createFrame(const XMLTree::Node &node);

    Buffer& getShaderBuffer(const std::string &name);
    Buffer& createShader(const XMLTree::Node &node);

    Buffer& getMeshBuffer(const std::string &name);
    Buffer& createMesh(const XMLTree::Node &node);
    
    Buffer& getTextureBuffer(const std::string &name);
    Buffer& createTexture(const XMLTree::Node &node);
    
    void clearBuffers();
    
    int getPassIndex(const std::string &name)
    {
      if (!mPassMap.count(name))
        mPassMap[name] = (int)mPassMap.size()+1;
      return mPassMap[name];
    }
    void clearPassIndices() {mPassMap.clear();}
    
    File directory() const {return mDirectory;}
    
  private:
    File mDirectory;
    
    IndexedMap<Buffer*> mFrameBuffers;
    IndexedMap<Buffer*> mShaderBuffers;
    IndexedMap<Buffer*> mMeshBuffers;
    IndexedMap<Buffer*> mTextureBuffers;
    
    std::map<std::string, int> mPassMap;
    
    GraphicSystem *mGraphicSystem;
  };
}

#endif /* Scene_h */
