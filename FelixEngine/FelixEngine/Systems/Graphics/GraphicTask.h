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


namespace fx {
  struct GraphicTask {
    FrameBuffer   *frame;
    ShaderProgram *shader;
    VertexMesh    *mesh;
  };
}

#endif /* GraphicTask_h */
