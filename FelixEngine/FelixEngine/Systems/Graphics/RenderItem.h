//
//  RenderItem.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/7/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef RenderItem_h
#define RenderItem_h

#include "Graphics.h"
#include "GraphicResources.h"
#include "GraphicStates.h"
#include "UniformMap.h"
#include "TextureMap.h"


namespace fx {
  /**
   *
   */
  struct RenderItem {
    ShaderProgramPtr shader;
    VertexMeshPtr    mesh;
    
    UniformsList uniforms;
    TexturesPtr textures;
    
    int instances;
    
    RenderItem() {}
  };
}

#endif /* RenderItem_h */
