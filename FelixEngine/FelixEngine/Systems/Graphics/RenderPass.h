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


namespace fx {
  
  /**
   *
   */
  typedef std::vector<RenderItem> RenderItems;
  
  /**
   *
   */
  struct RenderPass {
    FrameBufferPtr   frame;
    
    ActionState colorActions[MAX_COLOR_ATTACHEMENTS];
    ActionState depthStencilAction;
    
    CULL_MODE cullMode;
    DepthState depthState;
    BlendState blendState;
    
    UniformsPtr uniforms;
    
    RenderItems renderItems;
    
    RenderPass(): uniforms(UniformMap::make()) {}
    virtual ~RenderPass() {}
    
    void addRenderItem(const RenderItem &item) {renderItems.push_back(item);}
    virtual void render() = 0;
    
    Uniform& operator[](const std::string name) {return (*uniforms)[name];}
  };
}

#endif /* RenderPass_h */
