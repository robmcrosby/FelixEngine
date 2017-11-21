//
//  Shaders.metal
//  XMLScene
//
//  Created by Robert Crosby on 7/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include <metal_stdlib>

#define NUM_LIGHTS 3

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
  float4 direction;
  float4 color;
  float4 factors;
};


device float3 rotate_quat(float4 rot, float3 v);
device float light_attenuation(float distance, float2 factors);
device float shade_diffuse(float3 normal, float3 light);
device float shade_specular(float3 normal, float3 view, float3 light, float hardness);


struct VertexOutput {
  float4 position [[ position       ]];
  float3 location [[ user(location) ]];
  float3 normal   [[ user(normal)   ]];
  float3 view     [[ user(view)     ]];
};

struct FragmentInput {
  float3 location [[ user(location) ]];
  float3 normal   [[ user(normal)   ]];
  float3 view     [[ user(view)     ]];
};

device float3 rotate_quat(float4 rot, float3 v) {
  return v + cross(rot.xyz, (cross(rot.xyz, v) + v*rot.w))*2.0;
}

device float light_attenuation(float distance, float2 factors) {
  return 1.0 / (1.0 + distance*factors.x + distance*distance*factors.y);
}

device float shade_diffuse(float3 normal, float3 light) {
  return saturate(dot(normal, light));
}

device float shade_specular(float3 normal, float3 view, float3 light, float hardness) {
  float3 halfAngle = normalize(view + light);
  return pow(saturate(dot(normal, halfAngle)), hardness);
}

vertex VertexOutput basic_vertex(const device packed_float3 *position [[ buffer(0) ]],
                                 const device packed_float3 *normal   [[ buffer(1) ]],
                                 constant     Camera        *camera   [[ buffer(2) ]],
                                 constant     Model         *model    [[ buffer(3) ]],
                                 unsigned int   vid      [[ vertex_id ]]) {
  VertexOutput output;
  float4 location = model->model * float4(position[vid], 1.0);
  output.position = camera->projection * camera->view * location;
  output.location = location.xyz;
  output.normal = rotate_quat(model->rotation, float3(normal[vid]));
  output.view   = camera->position.xyz - output.position.xyz;
  return output;
}

fragment half4 basic_fragment(FragmentInput      input    [[ stage_in  ]],
                              constant Light    *lights   [[ buffer(0) ]],
                              constant Material *material [[ buffer(1) ]]) {
  // Set the base ambiant color
  float3 color = material->ambiant.xyz * material->ambiant.w;
  float3 normal = normalize(input.normal);
  float3 view = normalize(input.view);
  
  for (int i = 0; i < NUM_LIGHTS; ++i) {
    Light light = lights[i];
    float3 lightDirection = light.position.w ? light.position.xyz - input.location : -light.direction.xyz;
    float lightDistance = length(lightDirection);
    float attenuation = light_attenuation(lightDistance, light.factors.xy);
    
    lightDirection /= lightDistance;
    
    // Add Light Diffuse
    float diffuse = shade_diffuse(normal, lightDirection) * light.color.w * material->diffuse.w;
    color += material->diffuse.xyz * light.color.xyz * diffuse * attenuation;
    
    // Add Light Specular
    float specular = shade_specular(normal, view, lightDirection, material->factors.x) * light.color.w * material->specular.w;
    color += material->specular.xyz * light.color.xyz * specular * attenuation;
  }
  
  return half4(color.x, color.y, color.z, 1.0);
}
