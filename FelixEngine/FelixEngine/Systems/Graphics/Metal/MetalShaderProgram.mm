//
//  MetalShaderProgram.mm
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "MetalShaderProgram.h"
#include <Metal/Metal.h>


namespace fx {
  struct MTLShaderProgramData {
    id <MTLDevice> device;
    
    ~MTLShaderProgramData() {}
  };
}


using namespace fx;
using namespace std;


MetalShaderProgram::MetalShaderProgram(id <MTLDevice> device): _data(new MTLShaderProgramData()) {
  _data->device = device;
}

MetalShaderProgram::~MetalShaderProgram() {
  delete _data;
}

