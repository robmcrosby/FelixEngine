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



 float3 rotate_quat(float4 rot, float3 v);
 float2 transform_coordinate(float4x4 transform, float2 coordinate);
 float light_attenuation(float distance, float2 factors);

 void setupLightingParams(thread LightingParams *params, float3 normal, float3 view);
 void setLightToParams(thread LightingParams *params, float3 location, STR_Light light);
 void setupMaterialParams(thread MaterialParams *params, STR_Material material);
 void setupMaterialParams(thread MaterialParams *params, STR_Material material, float3 diffuse);

// Diffuse Shaders
float lambert(float3 normal, float3 light);
float oren_nayer(float3 normal, float3 light, float3 view, float roughness, float albedo);
float toon_d(float3 normal, float3 light, float size, float smooth);
float minnaert(float3 normal, float3 light, float3 view, float darkness);

// Specular Shaders
float phong(float3 normal, float3 light, float3 view, float hardness);
float cooktorr(float3 normal, float3 light, float3 view, float hardness);
float fresnel(float3 normal, float3 view, float power);
float toon_s(float3 normal, float3 light, float3 view, float size, float smooth);

float3 shade_lambert(LightingParams light, MaterialParams material);
float3 shade_oren_nayer(LightingParams light, MaterialParams material);
float3 shade_toon_d(LightingParams light, MaterialParams material);
float3 shade_minnaert(LightingParams light, MaterialParams material);

float3 shade_phong(LightingParams light, MaterialParams material);
float3 shade_cooktorr(LightingParams light, MaterialParams material);
float3 shade_toon_s(LightingParams light, MaterialParams material);



float fresnel2(float3 normal, float3 view) {
  float fresnel = 1.0 - saturate(dot(normal, view));
  return fresnel * fresnel;
}

float fresnel(float3 normal, float3 view, float3 ranges) {
  float result, f = fresnel2(normal, view);

  if (f > 0.5)
    result = mix(ranges.y, ranges.z, (2.0*f)-1.0);
  else
    result = mix(ranges.x, ranges.y, 2.0*f);
  return result;
}


float3 rotate_quat(float4 rot, float3 v) {
  return v + cross(rot.xyz, (cross(rot.xyz, v) + v*rot.w))*2.0;
}


float4 quat_inverse(const float4 q) {
 return float4(-q.xyz, q.w);
}

float4 quat_dot(const float4 q1, const float4 q2) {
 float scalar = q1.w * q2.w - dot(q1.xyz, q2.xyz);
 float3 v = cross(q1.xyz, q2.xyz) + q1.w * q2.xyz + q2.w * q1.xyz;
 return float4(v, scalar);
}

float3 quat_mult(const float4 q, const float3 v) {
 float4 r = quat_dot(q, quat_dot(float4(v, 0), quat_inverse(q)));
 return r.xyz;
}


float2 transform_coordinate(float4x4 transform, float2 coordinate) {
  return (transform * float4(coordinate, 0.0, 1.0)).xy;
}

float light_attenuation(float distance, float2 factors) {
  return 1.0 / (1.0 + distance*factors.x + distance*distance*factors.y);
}



void setupLightingParams(thread LightingParams *params, float3 normal, float3 view) {
  params->normal = normalize(normal);
  params->view = normalize(view);
}

void setLightToParams(thread LightingParams *params, float3 location, STR_Light light) {
  params->direction = light.position.w*(light.position.xyz - location) + (1.0 - light.position.w)*-light.direction.xyz;
  params->distance = length(params->direction);
  params->direction /= params->distance;
  params->attenuation = light_attenuation(params->distance, light.factors.xy);
  params->color = light.color.xyz;
  params->energy = light.color.w;
}

void setupMaterialParams(thread MaterialParams *params, STR_Material material) {
  params->ambiantColor = material.ambiant.xyz * material.ambiant.w;
  params->diffuseColor = material.diffuse.xyz * material.diffuse.w;
  params->specularColor = material.specular.xyz * material.specular.w;
  params->factors = material.factors;
}

void setupMaterialParams(thread MaterialParams *params, STR_Material material, float3 diffuse) {
  setupMaterialParams(params, material);
  params->diffuseColor *= diffuse;
}



// Diffuse Shaders
float lambert(float3 normal, float3 light) {
  return saturate(dot(normal, light));
}
float3 shade_lambert(LightingParams light, MaterialParams material) {
  float diffuse = lambert(light.normal, light.direction);
  return material.diffuseColor * light.color * diffuse * light.energy * light.attenuation;
}

float oren_nayer(float3 normal, float3 light, float3 view, float roughness, float albedo) {
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
float3 shade_oren_nayer(LightingParams light, MaterialParams material) {
  float diffuse = oren_nayer(light.normal, light.direction, light.view, material.factors.z, material.factors.w);
  return material.diffuseColor * light.color * diffuse * light.energy * light.attenuation;
}

float toon_d(float3 normal, float3 light, float size, float smooth) {
  float angle = acos(dot(normal, light));
  if (angle < size)
    return 1.0;
  if (angle > (size + smooth) || smooth == 0.0)
    return 0.0;
  return (1.0 - ((angle - size)/smooth));
}
float3 shade_toon_d(LightingParams light, MaterialParams material) {
  float diffuse = toon_d(light.normal, light.direction, material.factors.z, material.factors.w);
  return material.diffuseColor * light.color * diffuse * light.energy * light.attenuation;
}

float minnaert(float3 normal, float3 light, float3 view, float darkness) {
  float nl = max(dot(normal, light), 0.0);
  float nv = max(dot(normal, view), 0.0);
  
  if(darkness <= 1.0)
    return nl*pow(max(nv*nl, 0.1), darkness - 1.0);
  return nl*pow(1.0001 - nv, darkness - 1.0);
}
float3 shade_minnaert(LightingParams light, MaterialParams material) {
  float diffuse = minnaert(light.normal, light.direction, light.view, material.factors.z);
  return material.diffuseColor * light.color * diffuse * light.energy * light.attenuation;
}



// Specular Shaders
float phong(float3 normal, float3 light, float3 view, float hardness) {
  float3 halfAngle = normalize(view + light);
  return pow(saturate(dot(normal, halfAngle)), hardness);
}
float3 shade_phong(LightingParams light, MaterialParams material) {
  float specular = phong(light.normal, light.direction, light.view, material.factors.x);
  return material.specularColor * light.color * specular * light.energy * light.attenuation;
}

float cooktorr(float3 normal, float3 light, float3 view, float hardness) {
  float3 halfAngle = normalize(view + light);
  float nh = max(dot(normal, halfAngle), 0.0);
  float nv = max(dot(normal, view ), 0.0);
  return pow(nh, hardness)/(0.1+nv);
}
float3 shade_cooktorr(LightingParams light, MaterialParams material) {
  float specular = cooktorr(light.normal, light.direction, light.view, material.factors.x);
  return material.specularColor * light.color * specular * light.energy * light.attenuation;
}


float toon_s(float3 normal, float3 light, float3 view, float size, float smooth) {
  float3 halfAngle = normalize(view + light);
  float angle = acos(dot(halfAngle, normal));
  if (angle < size)
    return 1.0;
  if (angle >= (size + smooth) || smooth == 0.0)
    return 0.0;
  return (1.0 - ((angle - size)/smooth));
}
float3 shade_toon_s(LightingParams light, MaterialParams material) {
  float specular = toon_s(light.normal, light.direction, light.view, material.factors.x, material.factors.y);
  return material.specularColor * light.color * specular * light.energy * light.attenuation;
}


// Shlick's approximation of the Fresnel factor.
float3 fresnel_schlick(float3 F0, float cosTheta) {
  return F0 + (float3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}


// Importance sample GGX normal distribution function for a fixed roughness value.
// This returns normalized half-vector between Li & Lo.
// For derivation see: http://blog.tobias-franke.eu/2014/03/30/notes_on_importance_sampling.html
float3 sampleGGX(float u1, float u2, float roughness)
{
  float alpha = roughness * roughness;

  float cosTheta = sqrt((1.0 - u2) / (1.0 + (alpha*alpha - 1.0) * u2));
  float sinTheta = sqrt(1.0 - cosTheta*cosTheta); // Trig. identity
  float phi = 2.0 * PI * u1;

  // Convert to Cartesian upon return.
  return float3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
  return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method (IBL version).
float gaSchlickGGX_IBL(float cosLi, float cosLo, float roughness)
{
  float r = roughness;
  float k = (r * r) / 2.0; // Epic suggests using this roughness remapping for IBL lighting.
  return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

float radicalInverse_VdC(uint bits)
{
  bits = (bits << 16u) | (bits >> 16u);
  bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
  bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
  bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
  bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
  return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}


#endif /* MetalFunctions_h */
