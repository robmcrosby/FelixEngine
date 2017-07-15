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


device float3 rotate_quat(float4 rot, float3 v);
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

device float shade_diffuse(float3 normal, float3 light) {
  return saturate(dot(normal, light));
}

device float shade_specular(float3 normal, float3 view, float3 light, float hardness) {
  float3 halfAngle = normalize(view + light);
  return pow(saturate(dot(normal, halfAngle)), hardness);
}

vertex VertexOutput basic_vertex(const device packed_float4 *Position [[ buffer(0) ]],
                                 const device packed_float4 *Normal   [[ buffer(1) ]],
                                 constant     Camera        *camera   [[ buffer(2) ]],
                                 constant     Model         *model    [[ buffer(3) ]],
                                              unsigned int   vid      [[ vertex_id ]]) {
  VertexOutput output;
  float4 location = model->model * float4(Position[vid]);
  output.position = camera->projection * camera->view * location;
  output.location = location.xyz;
  output.normal = rotate_quat(model->rotation, float4(Normal[vid]).xyz);
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
  
  for (int i = 0; i < 3; ++i) {
    Light light = lights[i];
    float3 lightDirection = light.position.w ? light.position.xyz - input.location : -light.position.xyz;
    float lightDistance = length(lightDirection);
    
    lightDirection /= lightDistance;
    
    // Add Light Diffuse
    float diffuse = shade_diffuse(normal, lightDirection) * light.color.w * material->diffuse.w;
    color += material->diffuse.xyz * light.color.xyz * diffuse;
    
    // Add Light Specular
    float specular = shade_specular(normal, view, lightDirection, material->factors.x) * light.color.w * material->specular.w;
    color += material->specular.xyz * light.color.xyz * specular;
  }
  
  return half4(color.x, color.y, color.z, 1.0);
}
