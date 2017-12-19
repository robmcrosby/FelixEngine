//
//  MetalFunctions.h
//  FelixEngine
//
//  Created by Robert Crosby on 12/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef MetalFunctions_h
#define MetalFunctions_h

#include "GraphicStructures.h"

#define PI 3.14159265

#define LIGHT_LOOP(DIFFUSE, SPECULAR, START, END) \
  for (int i = START; i < END; ++i) { \
    setLightToParams(&lightParams, input.location, lights[i]); \
    color += shade_##DIFFUSE(lightParams, materialParams); \
    color += shade_##SPECULAR(lightParams, materialParams); \
  }



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
  float4 factors;
};



device float3 rotate_quat(float4 rot, float3 v);
device float2 transform_coordinate(float4x4 transform, float2 coordinate);
device float light_attenuation(float distance, float2 factors);

device void setupLightingParams(thread LightingParams *params, float3 normal, float3 view);
device void setLightToParams(thread LightingParams *params, float3 location, STR_Light light);
device void setupMaterialParams(thread MaterialParams *params, STR_Material material);
device void setupMaterialParams(thread MaterialParams *params, STR_Material material, float3 diffuse);

// Diffuse Shaders
device float lambert(float3 normal, float3 light);
device float oren_nayer(float3 normal, float3 light, float3 view, float roughness, float albedo);
device float toon_d(float3 normal, float3 light, float size, float smooth);
device float minnaert(float3 normal, float3 light, float3 view, float darkness);

// Specular Shaders
device float phong(float3 normal, float3 light, float3 view, float hardness);
device float cooktorr(float3 normal, float3 light, float3 view, float hardness);
device float toon_s(float3 normal, float3 light, float3 view, float size, float smooth);

device float3 shade_lambert(LightingParams light, MaterialParams material);
device float3 shade_oren_nayer(LightingParams light, MaterialParams material);
device float3 shade_toon_d(LightingParams light, MaterialParams material);
device float3 shade_minnaert(LightingParams light, MaterialParams material);

device float3 shade_phong(LightingParams light, MaterialParams material);
device float3 shade_cooktorr(LightingParams light, MaterialParams material);
device float3 shade_toon_s(LightingParams light, MaterialParams material);



device float3 rotate_quat(float4 rot, float3 v) {
  return v + cross(rot.xyz, (cross(rot.xyz, v) + v*rot.w))*2.0;
}

device float2 transform_coordinate(float4x4 transform, float2 coordinate) {
  return (transform * float4(coordinate, 0.0, 1.0)).xy;
}

device float light_attenuation(float distance, float2 factors) {
  return 1.0 / (1.0 + distance*factors.x + distance*distance*factors.y);
}



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
  params->factors = material.factors;
}

device void setupMaterialParams(thread MaterialParams *params, STR_Material material, float3 diffuse) {
  setupMaterialParams(params, material);
  params->diffuseColor *= diffuse;
}



// Diffuse Shaders
device float lambert(float3 normal, float3 light) {
  return saturate(dot(normal, light));
}
device float3 shade_lambert(LightingParams light, MaterialParams material) {
  float diffuse = lambert(light.normal, light.direction);
  return material.diffuseColor * light.color * diffuse * light.energy * light.attenuation;
}

device float oren_nayer(float3 normal, float3 light, float3 view, float roughness, float albedo) {
  float lv = dot(light, view);
  float nl = dot(light, normal);
  float nv = dot(normal, view);
  
  float s = lv - nl * nv;
  float t = mix(1.0, max(nl, nv), step(0.0, s));
  
  roughness *= roughness;
  float A = 1.0 + roughness*(albedo/(roughness + 0.13) + 0.5/(roughness + 0.33));
  float B = 0.45*roughness/(roughness + 0.09);
  
  return albedo * max(0.0, nl)*(A + B*s/t)/PI;
}
device float3 shade_oren_nayer(LightingParams light, MaterialParams material) {
  float diffuse = oren_nayer(light.normal, light.direction, light.view, material.factors.z, material.factors.w);
  return material.diffuseColor * light.color * diffuse * light.energy * light.attenuation;
}

device float toon_d(float3 normal, float3 light, float size, float smooth) {
  float angle = acos(dot(normal, light));
  if (angle < size)
    return 1.0;
  if (angle > (size + smooth) || smooth == 0.0)
    return 0.0;
  return (1.0 - ((angle - size)/smooth));
}
device float3 shade_toon_d(LightingParams light, MaterialParams material) {
  float diffuse = toon_d(light.normal, light.direction, material.factors.z, material.factors.w);
  return material.diffuseColor * light.color * diffuse * light.energy * light.attenuation;
}

device float minnaert(float3 normal, float3 light, float3 view, float darkness) {
  float nl = max(dot(normal, light), 0.0);
  float nv = max(dot(normal, view), 0.0);
  
  if(darkness <= 1.0)
    return nl*pow(max(nv*nl, 0.1), darkness - 1.0);
  return nl*pow(1.0001 - nv, darkness - 1.0);
}
device float3 shade_minnaert(LightingParams light, MaterialParams material) {
  float diffuse = minnaert(light.normal, light.direction, light.view, material.factors.z);
  return material.diffuseColor * light.color * diffuse * light.energy * light.attenuation;
}



// Specular Shaders
device float phong(float3 normal, float3 light, float3 view, float hardness) {
  float3 halfAngle = normalize(view + light);
  return pow(saturate(dot(normal, halfAngle)), hardness);
}
device float3 shade_phong(LightingParams light, MaterialParams material) {
  float specular = phong(light.normal, light.direction, light.view, material.factors.x);
  return material.specularColor * light.color * specular * light.energy * light.attenuation;
}

device float cooktorr(float3 normal, float3 light, float3 view, float hardness) {
  float3 halfAngle = normalize(view + light);
  float nh = max(dot(normal, halfAngle), 0.0);
  float nv = max(dot(normal, view ), 0.0);
  return pow(nh, hardness)/(0.1+nv);
}
device float3 shade_cooktorr(LightingParams light, MaterialParams material) {
  float specular = cooktorr(light.normal, light.direction, light.view, material.factors.x);
  return material.specularColor * light.color * specular * light.energy * light.attenuation;
}

device float toon_s(float3 normal, float3 light, float3 view, float size, float smooth) {
  float3 halfAngle = normalize(view + light);
  float angle = acos(dot(halfAngle, normal));
  if (angle < size)
    return 1.0;
  if (angle >= (size + smooth) || smooth == 0.0)
    return 0.0;
  return (1.0 - ((angle - size)/smooth));
}
device float3 shade_toon_s(LightingParams light, MaterialParams material) {
  float specular = toon_s(light.normal, light.direction, light.view, material.factors.x, material.factors.y);
  return material.specularColor * light.color * specular * light.energy * light.attenuation;
}



#endif /* MetalFunctions_h */
