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





struct TextureOutput {
  float4 position [[position]];
  float2 uv       [[user(uv)]];
};

struct TextureInput {
  float2 uv [[user(uv)]];
};

vertex TextureOutput camera_vertex(const device float4 *Vertices  [[ buffer(0) ]],
                                   unsigned     int     vid       [[ vertex_id ]]) {
  TextureOutput output;
  output.position = float4(Vertices[vid].xy, 0.0, 1.0);
  output.uv = Vertices[vid].zw;
  return output;
}

fragment float4 camera_fragment(TextureInput input [[ stage_in ]],
                                 texture2d<float, access::sample> capturedImageTextureY    [[ texture(0) ]],
                                 texture2d<float, access::sample> capturedImageTextureCbCr [[ texture(1) ]]) {
  
  constexpr sampler colorSampler(mip_filter::linear, mag_filter::linear, min_filter::linear);
  
  const float4x4 ycbcrToRGBTransform = float4x4(
                                                float4(+1.0000f, +1.0000f, +1.0000f, +0.0000f),
                                                float4(+0.0000f, -0.3441f, +1.7720f, +0.0000f),
                                                float4(+1.4020f, -0.7141f, +0.0000f, +0.0000f),
                                                float4(-0.7010f, +0.5291f, -0.8860f, +1.0000f)
                                                );
  
  // Sample Y and CbCr textures to get the YCbCr color at the given texture coordinate
  float4 ycbcr = float4(capturedImageTextureY.sample(colorSampler, input.uv).r,
                        capturedImageTextureCbCr.sample(colorSampler, input.uv).rg, 1.0);
  
  // Return converted RGB color
  return ycbcrToRGBTransform * ycbcr;
}

vertex TextureOutput texture_vertex(const device packed_float4 *Position  [[ buffer(0) ]],
                                    const device packed_float2 *UV        [[ buffer(1) ]],
                                    constant     float4x4      *Transform [[ buffer(2) ]],
                                    unsigned int                vid       [[ vertex_id ]]) {
  TextureOutput output;
  float4 texture = *Transform * float4(UV[vid], 0.0, 1.0);
  output.position = float4(Position[vid]);
  output.uv = texture.xy; //float2(UV[vid]);
  return output;
}

fragment float4 texture_fragment(TextureInput input [[ stage_in ]],
                                texture2d<float, access::sample> capturedImageTextureY    [[ texture(0) ]],
                                texture2d<float, access::sample> capturedImageTextureCbCr [[ texture(1) ]]) {
  
  constexpr sampler colorSampler(mip_filter::linear, mag_filter::linear, min_filter::linear);
  
  const float4x4 ycbcrToRGBTransform = float4x4(
      float4(+1.0000f, +1.0000f, +1.0000f, +0.0000f),
      float4(+0.0000f, -0.3441f, +1.7720f, +0.0000f),
      float4(+1.4020f, -0.7141f, +0.0000f, +0.0000f),
      float4(-0.7010f, +0.5291f, -0.8860f, +1.0000f)
  );
  
  // Sample Y and CbCr textures to get the YCbCr color at the given texture coordinate
  float4 ycbcr = float4(capturedImageTextureY.sample(colorSampler, input.uv).r,
                        capturedImageTextureCbCr.sample(colorSampler, input.uv).rg, 1.0);
  
  // Return converted RGB color
  return ycbcrToRGBTransform * ycbcr;
  //return float4(input.uv.x, input.uv.y, 1.0, 1.0);
}


