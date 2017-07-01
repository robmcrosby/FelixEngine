//
//  Shaders.metal
//  HelloTexture
//
//  Created by Robert Crosby on 7/1/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include <metal_stdlib>
using namespace metal;

struct MVPUniform {
  float4x4 projection;
  float4x4 view;
  float4x4 model;
};

struct VertexOutput {
  float4 position [[position]];
  float2 uv       [[user(uv)]];
};

struct FragmentInput {
  float2 uv [[user(uv)]];
};

vertex VertexOutput basic_vertex(const device packed_float4 *Position [[ buffer(0) ]],
                                 const device packed_float2 *UV       [[ buffer(1) ]],
                                 constant     MVPUniform    *MVP      [[ buffer(2) ]],
                                        unsigned int   vid      [[ vertex_id ]]) {
  VertexOutput output;
  output.position = MVP->projection * MVP->view * MVP->model * float4(Position[vid]);
  output.uv = float2(UV[vid]);
  return output;
}

fragment half4 basic_fragment(FragmentInput input [[ stage_in ]]) {
  float2 uv = input.uv;
  return half4(uv.x, uv.y, 1.0, 1.0);
}
