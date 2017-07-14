//
//  Shaders.metal
//  ExampleScene
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include <metal_stdlib>
using namespace metal;


device float3 rotate_quat(float4 rot, float3 v);

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

struct VertexOutput {
  float4 position [[position]];
  float3 normal [[user(normal)]];
};

struct FragmentInput {
  float3 normal [[user(normal)]];
};

device float3 rotate_quat(float4 rot, float3 v) {
  return v + cross(rot.xyz, (cross(rot.xyz, v) + v*rot.w))*2.0;
}

vertex VertexOutput basic_vertex(const device packed_float4 *Position [[ buffer(0) ]],
                                 const device packed_float4 *Normal   [[ buffer(1) ]],
                                 constant     Camera        *camera   [[ buffer(2) ]],
                                 constant     Model         *model    [[ buffer(3) ]],
                                              unsigned int   vid      [[ vertex_id ]]) {
  VertexOutput output;
  output.position = camera->projection * camera->view * model->model * float4(Position[vid]);
  output.normal = rotate_quat(model->rotation, float4(Normal[vid]).xyz);
  return output;
}

fragment half4 basic_fragment(FragmentInput input    [[ stage_in ]],
                     constant Light        *lights   [[ buffer(0) ]]) {
  float d = dot(input.normal, lights->position.xyz);
  return half4(d, d, d, 1.0);
}
