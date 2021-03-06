//
//  Shaders.metal
//  Hello3D
//
//  Created by Robert Crosby on 6/22/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include <metal_stdlib>
using namespace metal;

float3 rotate_quat(float4 rot, float3 v);

struct MVPUniform {
  float4x4 projection;
  float4x4 view;
  float4x4 model;
  float4   rotation;
};

struct VertexOutput {
  float4 position [[position]];
  float3 normal [[user(normal)]];
};

struct FragmentInput {
  float3 normal [[user(normal)]];
};

float3 rotate_quat(float4 rot, float3 v) {
  return v + cross(rot.xyz, (cross(rot.xyz, v) + v*rot.w))*2.0;
}

vertex VertexOutput basic_vertex(const device packed_float3 *position [[ buffer(0) ]],
                                 const device packed_float3 *normal   [[ buffer(1) ]],
                                 constant     MVPUniform    *MVP      [[ buffer(2) ]],
                                              unsigned int   vid      [[ vertex_id ]]) {
  VertexOutput output;
  output.normal = rotate_quat(MVP->rotation, float3(normal[vid]));
  output.position = MVP->projection * MVP->view * MVP->model * float4(position[vid], 1.0);
  return output;
}

fragment half4 basic_fragment(FragmentInput input [[ stage_in ]]) {
  float d = dot(input.normal, float3(0.0, 1.0, 0.0));
  return half4(d, d, d, 1.0);
}
