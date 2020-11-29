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

struct VertexInput {
  float3 position [[attribute(0)]];
  float2 uvMap    [[attribute(1)]];
};

struct VertexOutput {
  float4 position [[position]];
  float2 uv       [[user(uv)]];
};

struct FragmentInput {
  float2 uv [[user(uv)]];
};

vertex VertexOutput basic_vertex(VertexInput vert [[stage_in]], constant MVPUniform &MVP [[buffer(2)]]) {
  VertexOutput output;
  output.position = MVP.projection * MVP.view * MVP.model * float4(vert.position, 1.0);
  output.uv = vert.uvMap;
  return output;
}

//vertex VertexOutput basic_vertex(const device packed_float3 *position [[ buffer(0) ]],
//                                 const device packed_float2 *uvMap    [[ buffer(1) ]],
//                                 constant     MVPUniform    &MVP      [[ buffer(2) ]],
//                                              unsigned int   vid      [[ vertex_id ]]) {
//  VertexOutput output;
//  output.position = MVP.projection * MVP.view * MVP.model * float4(position[vid], 1.0);
//  output.uv = float2(uvMap[vid]);
//  return output;
//}

fragment half4 project_fragment(FragmentInput     input     [[ stage_in ]],
                              texture2d<float>  texture2D [[ texture(0) ]],
                              sampler           sampler2D [[ sampler(0) ]]) {
  
  // Right
  float3 loc(1 - 2*input.uv.x, 1, 1 - 2*input.uv.y);
  
  // Left
  //float3 loc(2*input.uv.x - 1, -1, 1 - 2*input.uv.y);
  
  // Front
  //float3 loc(1, 2*input.uv.x - 1, 1 - 2*input.uv.y);
  
  // Back
  //float3 loc(-1, 1 - 2*input.uv.x, 1 - 2*input.uv.y);
  
  // Top
  //float3 loc(2*input.uv.y - 1, 2*input.uv.x - 1, 1.0);
  
  // Bottom
  //float3 loc(1 - 2*input.uv.y, 2*input.uv.x - 1, -1.0);
  
  float theta = atan2(loc.y, loc.x);
  float r = sqrt(loc.x*loc.x + loc.y*loc.y);
  float phi = atan2(loc.z, r);
  
  float2 coords(0.5*(theta + M_PI_F) / M_PI_F, (M_PI_F/2 - phi) / M_PI_F);
  
  float4 color = texture2D.sample(sampler2D, coords);
  return half4(color.r, color.g, color.b, color.a);
}

fragment half4 basic_fragment(FragmentInput     input     [[ stage_in ]],
                              texture2d<float>  texture2D [[ texture(0) ]],
                              sampler           sampler2D [[ sampler(0) ]]) {
  float4 color = texture2D.sample(sampler2D, input.uv);
  //float4 color = texture2D.sample(sampler2D, input.uv, level(5.0));
  return half4(color.r, color.g, color.b, color.a);
}
