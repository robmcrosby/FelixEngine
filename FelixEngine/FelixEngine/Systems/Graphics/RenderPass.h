//
//  RenderPass.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/7/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef RenderPass_h
#define RenderPass_h

#include "iObject.h"
#include "RenderItem.h"
#include "Graphics.h"
#include "XMLTree.h"
#include <set>


namespace fx {
  
  /**
   *
   */
  typedef std::vector<RenderItem> RenderItems;
  
  /**
   *
   */
  struct RenderPass: public iObject {
    Scene *_scene;
    FrameBufferPtr frame;
    
    ActionState colorActions[MAX_COLOR_ATTACHEMENTS];
    ActionState depthStencilAction;
    
    UniformsList uniforms;
    
    bool sortItems;
    bool itemsSorted;
    RenderItems renderItems;
    
    RenderPass();
    virtual ~RenderPass();
    
    void addRenderItem(const RenderItem &item);
    void clearRenderItems();
    
    virtual void render() = 0;
    
    FrameBufferPtr getFrame();
    ivec2 getFrameSize();
    void setFrame(const std::string &name);
    bool setFrame(const XMLTree::Node &node);
    bool setFrameToWindow(int index);
    bool resizeFrame(int width, int height);
    bool addColorTexture();
    bool addDepthBuffer();
    TextureBufferPtr getColorTexture(int index = 0);
    
    bool setClearSettings(const XMLTree::Node &node);
    void setClearColor(const vec4 &color);
    void setDefaultColorActions();
    void setClearDepthStencil(float depth = 1.0f, int stencil = 0);
    void setDefaultDepthStencilActions();
    
    void addUniformMap(UniformsPtr uniformMap);
    UniformMap& getUniformMap(int index = 0);
    void clearUniformMaps();
    
    virtual void setScene(Scene *scene);
    virtual void update(float dt) {}
    
    virtual bool loadXML(const XMLTree::Node &node);
    bool loadXMLItem(const XMLTree::Node &node);
    
    void sortRenderItems();
  };
}

#endif /* RenderPass_h */
