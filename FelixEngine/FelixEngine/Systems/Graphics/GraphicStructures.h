//
//  GraphicStructures.h
//  FelixEngine
//
//  Created by Robert Crosby on 11/28/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef GraphicStructures_h
#define GraphicStructures_h

#ifdef __METAL_VERSION__
#include <metal_stdlib>
using namespace metal;
#else
#include "Matrix.h"
using namespace fx;
#endif


struct Model {
  float4x4 model;
  float4   rotation;
};

struct Camera {
  float4x4 projection;
  float4x4 view;
  float4   position;
};

struct Material {
  float4 ambiant;
  float4 diffuse;
  float4 specular;
  float4 factors;
};

struct Light {
  float4 position;
  float4 direction;
  float4 color;
  float4 factors;
};

#endif /* GraphicStructures_h */
