//
//  MetalShaders.metal
//  FelixEngine
//
//  Created by Robert Crosby on 11/29/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#define NUM_LIGHTS 2
#define FOUR_LIGHTS 4

#include "GraphicStructures.h"


device float3 rotate_quat(float4 rot, float3 v);
device float2 transform_coordinate(float4x4 transform, float2 coordinate);
device float light_attenuation(float distance, float2 factors);
device float shade_diffuse(float3 normal, float3 light);
device float shade_specular(float3 normal, float3 view, float3 light, float hardness);


device float3 rotate_quat(float4 rot, float3 v) {
  return v + cross(rot.xyz, (cross(rot.xyz, v) + v*rot.w))*2.0;
}

device float2 transform_coordinate(float4x4 transform, float2 coordinate) {
  return (transform * float4(coordinate, 0.0, 1.0)).xy;
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




struct OutputShadeless {
  float4 position [[ position ]];
};

vertex OutputShadeless v_shadeless(const device packed_float3 *position [[ buffer(0) ]],
                                   constant     STR_Camera*    camera   [[ buffer(1) ]],
                                   constant     STR_Model*     model    [[ buffer(2) ]],
                                   unsigned     int            vid      [[ vertex_id ]]) {
  OutputShadeless output;
  float4 location = model->model * float4(position[vid], 1.0);
  output.position = camera->projection * camera->view * location;
  return output;
}

fragment half4 f_shadeless(constant STR_Material *material [[ buffer(0) ]]) {
  float3 color = material->ambiant.xyz * material->ambiant.a;
  return half4(color.x, color.y, color.z, 1.0);
}



struct OutputColor {
  float4 position [[ position    ]];
  float3 color    [[ user(color) ]];
};

struct InputColor {
  float3 color [[ user(color) ]];
};

vertex OutputColor v_color(const device packed_float3* position [[ buffer(0) ]],
                           const device packed_float3* color0   [[ buffer(1) ]],
                           constant     STR_Camera*    camera   [[ buffer(2) ]],
                           constant     STR_Model*     model    [[ buffer(3) ]],
                           unsigned     int            vid      [[ vertex_id ]]) {
  OutputColor output;
  float4 location = model->model * float4(position[vid], 1.0);
  output.position = camera->projection * camera->view * location;
  output.color = float3(color0[vid]);
  return output;
}

fragment half4 f_color_shadeless(InputColor             input     [[ stage_in ]],
                                 constant STR_Material* material  [[ buffer(0) ]]) {
  float3 color = input.color * material->ambiant.xyz * material->ambiant.a;
  return half4(color.x, color.y, color.z, 1.0);
}




struct OutputTexture {
  float4 position   [[ position ]];
  float2 coordinate [[ user(uv) ]];
};

struct InputTexture {
  float2 coordinate [[ user(uv) ]];
};

vertex OutputTexture v_texture(const device packed_float3* position   [[ buffer(0) ]],
                               const device packed_float2* uv0        [[ buffer(1) ]],
                               constant     STR_Camera*    camera     [[ buffer(2) ]],
                               constant     STR_Model*     model      [[ buffer(3) ]],
                               unsigned     int            vid        [[ vertex_id ]]) {
  OutputTexture output;
  float4 location = model->model * float4(position[vid], 1.0);
  output.position = camera->projection * camera->view * location;
  output.coordinate = transform_coordinate(model->texture, float2(uv0[vid]));
  return output;
}

fragment half4 f_texture_shadeless(InputTexture           input     [[ stage_in ]],
                                   constant STR_Material* material  [[ buffer(0) ]],
                                   texture2d<float>       texture2D [[ texture(0) ]],
                                   sampler                sampler2D [[ sampler(0) ]]) {
  float4 texture = texture2D.sample(sampler2D, input.coordinate);
  float3 color = texture.xyz * material->ambiant.xyz * material->ambiant.a;
  return half4(color.x, color.y, color.z, 1.0);
}







struct OutputNormal {
  float4 position [[ position   ]];
  float3 location [[ user(loc)  ]];
  float3 normal   [[ user(norm) ]];
  float3 view     [[ user(view) ]];
};

struct InputNormal {
  float3 location [[ user(loc)  ]];
  float3 normal   [[ user(norm) ]];
  float3 view     [[ user(view) ]];
};

struct LightingParams {
  float3 normal;
  float3 view;
  float3 direction;
  float3 color;
  float distance;
  float attenuation;
  float energy;
};

struct MaterialParams {
  float3 ambiantColor;
  float3 diffuseColor;
  float3 specularColor;
  float specularHardness;
};

device void setupLightingParams(thread LightingParams *params, float3 normal, float3 view) {
  params->normal = normalize(normal);
  params->view = normalize(view);
}

device void setLightToParams(thread LightingParams *params, float3 location, STR_Light light) {
  params->direction = light.position.w*(light.position.xyz - location) + (1.0 - light.position.w)*-light.direction.xyz;
  params->distance = length(params->direction);
  params->direction /= params->distance;
  params->attenuation = light_attenuation(params->distance, light.factors.xy);
  params->color = light.color.xyz;
  params->energy = light.color.w;
}

device void setupMaterialParams(thread MaterialParams *params, STR_Material material) {
  params->ambiantColor = material.ambiant.xyz * material.ambiant.w;
  params->diffuseColor = material.diffuse.xyz * material.diffuse.w;
  params->specularColor = material.specular.xyz * material.specular.w;
  params->specularHardness = material.factors.x;
}

device void setupMaterialParams(thread MaterialParams *params, STR_Material material, float3 diffuse) {
  setupMaterialParams(params, material);
  params->diffuseColor *= diffuse;
}

device float lambert(LightingParams params) {
  return saturate(dot(params.normal, params.direction)) * params.energy * params.attenuation;
}
device float3 shadeLambert(LightingParams light, MaterialParams material) {
  return material.diffuseColor * light.color * lambert(light);
}

device float phong(LightingParams params, float hardness) {
  float3 halfAngle = normalize(params.view + params.direction);
  return pow(saturate(dot(params.normal, halfAngle)), hardness) * params.energy * params.attenuation;
}
device float3 shadePhong(LightingParams light, MaterialParams material) {
  return material.specularColor * light.color * phong(light, material.specularHardness);
}



vertex OutputNormal v_normal(const device packed_float3 *position [[ buffer(0) ]],
                             const device packed_float3 *normal   [[ buffer(1) ]],
                             constant     STR_Camera    *camera   [[ buffer(2) ]],
                             constant     STR_Model     *model    [[ buffer(3) ]],
                             unsigned     int            vid      [[ vertex_id ]]) {
  OutputNormal output;
  float4 location = model->model * float4(position[vid], 1.0);
  output.position = camera->projection * camera->view * location;
  output.location = location.xyz;
  output.normal = rotate_quat(model->rotation, float3(normal[vid]));
  output.view = camera->position.xyz - output.position.xyz;
  return output;
}

fragment half4 f_lambert_phong_4(InputNormal            input    [[ stage_in  ]],
                                 constant STR_Light    *lights   [[ buffer(0) ]],
                                 constant STR_Material *material [[ buffer(1) ]]) {
  LightingParams lightParams;
  MaterialParams materialParams;
  
  setupLightingParams(&lightParams, input.normal, input.view);
  setupMaterialParams(&materialParams, *material);
  
  float3 color = materialParams.ambiantColor;
  for (int i = 0; i < NUM_LIGHTS; ++i) {
    setLightToParams(&lightParams, input.location, lights[i]);
    color += shadeLambert(lightParams, materialParams);
    color += shadePhong(lightParams, materialParams);
  }
  
  return half4(color.x, color.y, color.z, 1.0);
}





struct OutputNormalColor {
  float4 position [[ position    ]];
  float3 location [[ user(loc)   ]];
  float3 normal   [[ user(norm)  ]];
  float3 view     [[ user(view)  ]];
  float3 color    [[ user(color) ]];
};

struct InputNormalColor {
  float3 location [[ user(loc)   ]];
  float3 normal   [[ user(norm)  ]];
  float3 view     [[ user(view)  ]];
  float3 color    [[ user(color) ]];
};

vertex OutputNormalColor v_color_normal(const device packed_float3 *position [[ buffer(0) ]],
                                        const device packed_float3 *normal   [[ buffer(1) ]],
                                        const device packed_float3 *color0   [[ buffer(2) ]],
                                        constant     STR_Camera    *camera   [[ buffer(3) ]],
                                        constant     STR_Model     *model    [[ buffer(4) ]],
                                        unsigned     int            vid      [[ vertex_id ]]) {
  OutputNormalColor output;
  float4 location = model->model * float4(position[vid], 1.0);
  output.position = camera->projection * camera->view * location;
  output.location = location.xyz;
  output.normal = rotate_quat(model->rotation, float3(normal[vid]));
  output.view = camera->position.xyz - output.position.xyz;
  output.color = float3(color0[vid]);
  return output;
}

fragment half4 f_color_lambert_phong_4(InputNormalColor       input    [[ stage_in  ]],
                                       constant STR_Light    *lights   [[ buffer(0) ]],
                                       constant STR_Material *material [[ buffer(1) ]]) {
  LightingParams lightParams;
  MaterialParams materialParams;
  
  setupLightingParams(&lightParams, input.normal, input.view);
  setupMaterialParams(&materialParams, *material, input.color);
  
  float3 color = materialParams.ambiantColor;
  for (int i = 0; i < NUM_LIGHTS; ++i) {
    setLightToParams(&lightParams, input.location, lights[i]);
    color += shadeLambert(lightParams, materialParams);
    color += shadePhong(lightParams, materialParams);
  }
  
  return half4(color.x, color.y, color.z, 1.0);
}










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

vertex VertexOutput basic_vertex(const device packed_float3 *position [[ buffer(0) ]],
                                 const device packed_float3 *normal   [[ buffer(1) ]],
                                 constant     STR_Camera    *camera   [[ buffer(2) ]],
                                 constant     STR_Model     *model    [[ buffer(3) ]],
                                 unsigned     int            vid      [[ vertex_id ]]) {
  VertexOutput output;
  float4 location = model->model * float4(position[vid], 1.0);
  output.position = camera->projection * camera->view * location;
  output.location = location.xyz;
  output.normal = rotate_quat(model->rotation, float3(normal[vid]));
  output.view   = camera->position.xyz - output.position.xyz;
  return output;
}

fragment half4 basic_fragment(FragmentInput          input    [[ stage_in  ]],
                              constant STR_Light    *lights   [[ buffer(0) ]],
                              constant STR_Material *material [[ buffer(1) ]]) {
  // Set the base ambiant color
  float3 color = material->ambiant.xyz * material->ambiant.w;
  float3 normal = normalize(input.normal);
  float3 view = normalize(input.view);
  
  for (int i = 0; i < NUM_LIGHTS; ++i) {
    STR_Light light = lights[i];
    float3 lightDirection = light.position.w*(light.position.xyz - input.location) + (1.0 - light.position.w)*-light.direction.xyz;
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