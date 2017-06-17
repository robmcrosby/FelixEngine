//
//  MetalShaderProgram.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "GraphicResources.h"

@protocol MTLDevice;


namespace fx {
  class MTLShaderProgramData;
  
  class MetalShaderProgram: public ShaderProgram {
  private:
    MTLShaderProgramData *_data;
    
  public:
    MetalShaderProgram(id <MTLDevice> device);
    virtual ~MetalShaderProgram();
  };
}
