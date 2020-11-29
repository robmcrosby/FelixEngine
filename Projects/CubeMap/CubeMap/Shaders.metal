//
//  Shaders.metal
//  CubeMap
//
//  Created by Robert Crosby on 11/25/20.
//

#include <metal_stdlib>
using namespace metal;

float3 rotate_quat(float4 rot, float3 v);

struct MVPUniform {
  float4x4 projection;
  float4x4 view;
  float4x4 model;
  float4   rotation;
  float4   camera;
};

struct VertexInput {
  float3 position [[attribute(0)]];
  float3 normal   [[attribute(1)]];
};

struct VertexOutput {
  float4 position [[position]];
  float3 texCoords [[user(tex)]];
};

struct FragmentInput {
  float3 texCoords [[user(tex)]];
};

float3 rotate_quat(float4 rot, float3 v) {
  return v + cross(rot.xyz, (cross(rot.xyz, v) + v*rot.w))*2.0;
}

vertex VertexOutput skybox_vertex(VertexInput vert [[stage_in]],
                        constant MVPUniform  &MVP [[buffer(2)]]) {
  VertexOutput output;
  output.position = MVP.projection * MVP.view * MVP.model * float4(vert.position, 1.0);
  
  //float3 normal = normalize(rotate_quat(MVP.rotation, vert.normal));
  float3 location = (MVP.model * float4(vert.position, 1.0)).xyz;
  float3 view = normalize(location - MVP.camera.xyz);
  //float3 view = normalize(MVP.camera.xyz);
  output.texCoords = view; //reflect(view, normal);
  return output;
}


vertex VertexOutput basic_vertex(VertexInput vert [[stage_in]],
                        constant MVPUniform  &MVP [[buffer(2)]]) {
  VertexOutput output;
  output.position = MVP.projection * MVP.view * MVP.model * float4(vert.position, 1.0);
  
  float3 normal = normalize(rotate_quat(MVP.rotation, vert.normal));
  //float3 location = (MVP.model * float4(vert.position, 1.0)).xyz;
  //float3 view = normalize(MVP.camera.xyz - location);
  float3 view = normalize(MVP.camera.xyz);
  output.texCoords = reflect(view, normal);
  return output;
}

fragment half4 basic_fragment(FragmentInput     input       [[stage_in]],
                              texturecube<half> cubeMap     [[texture(0)]],
                              sampler           cubeSampler [[sampler(0)]]) {
  //float d = dot(input.normal, float3(0.0, 0.0, 1.0));
  //return half4(d, d, d, 1.0);
  //return cubeMap.sample(cubeSampler, input.texCoords, level(8));
  return cubeMap.sample(cubeSampler, input.texCoords);
}
