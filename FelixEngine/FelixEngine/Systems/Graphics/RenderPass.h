//
//  RenderPass.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/7/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef RenderPass_h
#define RenderPass_h

#include "RenderItem.h"
#include "Graphics.h"
#include <set>


namespace fx {
  
  /**
   *
   */
  typedef std::vector<RenderItem> RenderItems;
  
  /**
   *
   */
  typedef std::set<RenderPassPtr> RenderPasses;
  
  /**
   *
   */
  struct RenderPass {
    FrameBufferPtr frame;
    
    ActionState colorActions[MAX_COLOR_ATTACHEMENTS];
    ActionState depthStencilAction;
    
    UniformsList uniforms;
    
    RenderItems renderItems;
    
    RenderPass() {}
    virtual ~RenderPass() {}
    
    void addRenderItem(const RenderItem &item) {renderItems.push_back(item);}
    void clearRenderItems() {renderItems.clear();}
    
    virtual void render() = 0;
    
    FrameBufferPtr getFrame() {
      if (!frame)
        frame = Graphics::getInstance().createFrameBuffer();
      return frame;
    }
    ivec2 getFrameSize() {return getFrame()->size();}
    void setFrame(const std::string &name) {frame = Graphics::getInstance().getFrameBuffer(name);}
    bool setFrameToWindow(int index) {return getFrame()->setToWindow(index);}
    bool resizeFrame(int width, int height) {return getFrame()->resize(width, height);}
    bool addColorTexture() {return getFrame()->addColorTexture();}
    bool addDepthBuffer() {return getFrame()->addDepthBuffer();}
    TextureBufferPtr getColorTexture(int index = 0) {return getFrame()->getColorTexture(0);}
    
    
    void setClearColor(const vec4 &color) {
      for (int i = 0; i < MAX_COLOR_ATTACHEMENTS; ++i) {
        colorActions[i].loadAction = LOAD_CLEAR;
        colorActions[i].clearColor = color;
      }
    }
    void setDefaultColorActions() {
      for (int i = 0; i < MAX_COLOR_ATTACHEMENTS; ++i) {
        colorActions[i].loadAction = LOAD_LAST;
        colorActions[i].storeAction = STORE_SAVE;
        colorActions[i].clearColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
      }
    }
    void setClearDepthStencil(float depth = 1.0f, int stencil = 0) {
      depthStencilAction.loadAction = LOAD_CLEAR;
      depthStencilAction.clearColor = vec4(depth, (float)stencil, 0.0f, 1.0f);
    }
    void setDefaultDepthStencilActions() {
      depthStencilAction.loadAction = LOAD_LAST;
      depthStencilAction.storeAction = STORE_SAVE;
      depthStencilAction.clearColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    
    void addUniformMap(UniformsPtr uniformMap) {uniforms.push_back(uniformMap);}
    UniformMap& getUniformMap(int index = 0) {
      while (uniforms.size() <= index)
        uniforms.push_back(UniformMap::make());
      return *uniforms.at(index).get();
    }
    void clearUniformMaps() {uniforms.clear();}
  };
}

#endif /* RenderPass_h */
