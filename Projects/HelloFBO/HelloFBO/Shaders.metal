//
//  Shaders.metal
//  HelloFBO
//
//  Created by Robert Crosby on 7/4/17.
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

struct VertexInput {
  float4 Position [[attribute(0)]];
  float4 Normal   [[attribute(1)]];
};

struct NormalOutput {
  float4 position [[position]];
  float3 normal [[user(normal)]];
};

struct NormalInput {
  float3 normal [[user(normal)]];
};

float3 rotate_quat(float4 rot, float3 v) {
  return v + cross(rot.xyz, (cross(rot.xyz, v) + v*rot.w))*2.0;
}

vertex NormalOutput basic_vertex(VertexInput vert [[stage_in]], constant MVPUniform &MVP [[buffer(2)]]) {
  NormalOutput output;
  output.normal = rotate_quat(MVP.rotation, vert.Normal.xyz);
  output.position = MVP.projection * MVP.view * MVP.model * float4(vert.Position);
  return output;
}

fragment half4 basic_fragment(NormalInput input [[ stage_in ]]) {
  float d = dot(input.normal, float3(0.0, 1.0, 0.0));
  return half4(d, d, d, 1.0);
}





struct UVOutput {
  float4 position [[position]];
  float2 uv       [[user(uv)]];
};

struct UVInput {
  float2 uv [[user(uv)]];
};

vertex UVOutput texture_vertex(const device packed_float4 *Position [[ buffer(0) ]],
                               const device packed_float2 *UV       [[ buffer(1) ]],
                               constant     MVPUniform    *MVP      [[ buffer(2) ]],
                                            unsigned int   vid      [[ vertex_id ]]) {
  UVOutput output;
  output.position = MVP->projection * MVP->view * MVP->model * float4(Position[vid]);
  output.uv = float2(UV[vid]);
  return output;
}

fragment half4 texture_fragment(UVInput           input     [[ stage_in ]],
                                texture2d<float>  texture2D [[ texture(0) ]],
                                sampler           sampler2D [[ sampler(0) ]]) {
  float4 color = texture2D.sample(sampler2D, input.uv);
  return half4(color.r, color.g, color.b, color.a);
}
