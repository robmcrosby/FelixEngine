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

vertex VertexOutput basic_vertex(const device packed_float3 *position [[ buffer(0) ]],
                                 const device packed_float2 *uvMap    [[ buffer(1) ]],
                                 constant     MVPUniform    *MVP      [[ buffer(2) ]],
                                              unsigned int   vid      [[ vertex_id ]]) {
  VertexOutput output;
  output.position = MVP->projection * MVP->view * MVP->model * float4(position[vid], 1.0);
  output.uv = float2(uvMap[vid]);
  return output;
}

fragment half4 basic_fragment(FragmentInput     input     [[ stage_in ]],
                              texture2d<float>  texture2D [[ texture(0) ]],
                              sampler           sampler2D [[ sampler(0) ]]) {
  float4 color = texture2D.sample(sampler2D, input.uv);
  return half4(color.r, color.g, color.b, color.a);
}
