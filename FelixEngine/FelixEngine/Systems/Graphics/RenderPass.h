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


namespace fx {
  
  /**
   *
   */
  typedef std::vector<RenderItem> RenderItems;
  
  /**
   *
   */
  struct RenderPass {
    FrameBufferPtr frame;
    
    ActionState colorActions[MAX_COLOR_ATTACHEMENTS];
    ActionState depthStencilAction;
    
    UniformsPtr uniforms;
    
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
    void setFrame(const std::string &name) {frame = Graphics::getInstance().getFrameBuffer(name);}
    bool setFrameToWindow(int index) {return getFrame()->setToWindow(index);}
    
    void setClearColor(const vec4 &color) {
      for (int i = 0; i < MAX_COLOR_ATTACHEMENTS; ++i) {
        colorActions[i].loadAction = LOAD_CLEAR;
        colorActions[i].clearColor = color;
      }
    }
    
    UniformsPtr getUniformMap() {
      if (!uniforms)
        uniforms = UniformMap::make();
      return uniforms;
    }
    Uniform& operator[](const std::string name) {return (*getUniformMap())[name];}
  };
}

#endif /* RenderPass_h */
