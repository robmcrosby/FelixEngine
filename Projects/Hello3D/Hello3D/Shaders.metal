//
//  Shaders.metal
//  Hello3D
//
//  Created by Robert Crosby on 6/22/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include <metal_stdlib>
using namespace metal;

device float3 rotate_quat(float4 rot, float3 v);

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

device float3 rotate_quat(float4 rot, float3 v) {
  return v + cross(rot.xyz, (cross(rot.xyz, v) + v*rot.w))*2.0;
}

vertex VertexOutput basic_vertex(const device packed_float4 *Position [[ buffer(0) ]],
                                 const device packed_float4 *Normal   [[ buffer(1) ]],
                                 constant     MVPUniform    *MVP      [[ buffer(2) ]],
                                              unsigned int   vid      [[ vertex_id ]]) {
  VertexOutput output;
  float4 normal = Normal[vid];
  output.normal = rotate_quat(MVP->rotation, normal.xyz);
  output.position = MVP->projection * MVP->view * MVP->model * float4(Position[vid]);
  return output;
}

fragment half4 basic_fragment(FragmentInput input [[ stage_in ]]) {
  float d = dot(input.normal, float3(0.0, 1.0, 0.0));
  return half4(d, d, d, 1.0);
}
