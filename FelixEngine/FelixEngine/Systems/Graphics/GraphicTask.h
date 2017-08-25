//
//  GraphicTask.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/17/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef GraphicTask_h
#define GraphicTask_h

#include "Graphics.h"
#include "GraphicResources.h"
#include "GraphicStates.h"
#include "UniformMap.h"
#include "TextureMap.h"


namespace fx {
  struct GraphicTask {
    FrameBuffer   *frame;
    ShaderProgram *shader;
    VertexMesh    *mesh;
    
    UniformMap *uniforms;
    TextureMap *textures;
    
    int instances;
    
    ActionState colorActions[MAX_COLOR_ATTACHEMENTS];
    ActionState depthStencilAction;
    
    CULL_MODE cullMode;
    DepthState depthState;
    BlendState blendState;
    
    GraphicTask() {
      frame  = nullptr;
      shader = nullptr;
      mesh   = nullptr;
      
      uniforms = nullptr;
      textures = nullptr;
      
      instances = 1;
      
      cullMode = CULL_NONE;
    }
    
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
    
    void enableDepthTesting() {depthState.enableDefaultTesting();}
    void enableBlending() {blendState.enableDefaultBlending();}
  };
}

#endif /* GraphicTask_h */
