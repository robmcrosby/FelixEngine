//
//  Shaders.metal
//  ExampleScene
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include <metal_stdlib>
using namespace metal;


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
  float4 color;
  float4 factors;
};



vertex float4 basic_vertex(const device packed_float4* vertex_array [[ buffer(0) ]],
                           unsigned int   vid          [[ vertex_id ]]) {
  return float4(vertex_array[vid]);
}

fragment half4 basic_fragment() {
  return half4(1.0);
}
