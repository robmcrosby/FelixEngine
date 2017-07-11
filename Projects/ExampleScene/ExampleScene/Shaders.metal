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



vertex float4 basic_vertex(const device packed_float4 *Position [[ buffer(0) ]],
                           constant     Camera        *camera   [[ buffer(1) ]],
                           constant     Model         *model    [[ buffer(2) ]],
                                        unsigned int   vid      [[ vertex_id ]]) {
  return camera->projection * camera->view * model->model * float4(Position[vid]);
}

fragment half4 basic_fragment() {
  return half4(1.0);
}
