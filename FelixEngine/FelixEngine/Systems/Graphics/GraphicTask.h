//
//  GraphicTask.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/17/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef GraphicTask_h
#define GraphicTask_h

#include "GraphicResources.h"
#include "GraphicStates.h"
#include <vector>
#include <map>


namespace fx {
  typedef std::map<std::string, UniformBuffer*> UniformMap;
  
  struct GraphicTask {
    FrameBuffer   *frame;
    ShaderProgram *shader;
    VertexMesh    *mesh;
    
    int instances;
    
    AttachmentState colorAttachments[MAX_COLOR_ATTACHEMENTS];
    AttachmentState depthStencilAttachment;
    
    UniformMap uniforms;
    
    CULL_MODE cullMode;
    DepthStencilState depthStencilState;
    
    GraphicTask() {
      frame  = nullptr;
      shader = nullptr;
      mesh   = nullptr;
      
      instances = 1;
      
      cullMode = CULL_NONE;
    }
    
    void setClearColor(const vec4 &color) {
      for (int i = 0; i < MAX_COLOR_ATTACHEMENTS; ++i) {
        colorAttachments[i].loadAction = LOAD_CLEAR;
        colorAttachments[i].clearColor = color;
      }
    }
    
    void setClearDepthStencil(float depth = 1.0f, int stencil = 0) {
      depthStencilAttachment.loadAction = LOAD_CLEAR;
      depthStencilAttachment.clearColor = vec4(depth, (float)stencil, 0.0f, 0.0f);
    }
  };
}

#endif /* GraphicTask_h */
