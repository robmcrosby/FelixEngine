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

#define PI 3.14159265

device float3 rotate_quat(float4 rot, float3 v);
device float2 transform_coordinate(float4x4 transform, float2 coordinate);
device float light_attenuation(float distance, float2 factors);


device float3 rotate_quat(float4 rot, float3 v) {
  return v + cross(rot.xyz, (cross(rot.xyz, v) + v*rot.w))*2.0;
}

device float2 transform_coordinate(float4x4 transform, float2 coordinate) {
  return (transform * float4(coordinate, 0.0, 1.0)).xy;
}

device float light_attenuation(float distance, float2 factors) {
  return 1.0 / (1.0 + distance*factors.x + distance*distance*factors.y);
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
  float4 factors;
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
  params->factors = material.factors;
}

device void setupMaterialParams(thread MaterialParams *params, STR_Material material, float3 diffuse) {
  setupMaterialParams(params, material);
  params->diffuseColor *= diffuse;
}

// Diffuse Shaders
device float lambert(LightingParams params) {
  return saturate(dot(params.normal, params.direction)) * params.energy * params.attenuation;
}
device float3 shade_lambert(LightingParams light, MaterialParams material) {
  return material.diffuseColor * light.color * lambert(light);
}

device float oren_nayer(LightingParams params, float roughness, float albedo) {
  float LdotV = dot(params.direction, params.view);
  float NdotL = dot(params.direction, params.normal);
  float NdotV = dot(params.normal, params.view);
  
  float s = LdotV - NdotL * NdotV;
  float t = mix(1.0, max(NdotL, NdotV), step(0.0, s));
  
  roughness *= roughness;
  float A = 1.0 + roughness*(albedo/(roughness + 0.13) + 0.5/(roughness + 0.33));
  float B = 0.45*roughness/(roughness + 0.09);
  
  return albedo * max(0.0, NdotL)*(A + B*s/t)/PI * params.energy * params.attenuation;
}
device float3 shade_oren_nayer(LightingParams light, MaterialParams material) {
  return material.diffuseColor * light.color * oren_nayer(light, material.factors.z, material.factors.w);
}

device float toon_d(LightingParams params, float size, float smooth) {
  float angle = acos(dot(params.normal, params.direction));
  if (angle < size)
    return params.energy * params.attenuation;
  if (angle > (size + smooth) || smooth == 0.0)
    return 0.0;
  return (1.0 - ((angle - size)/smooth)) * params.energy * params.attenuation;
}
device float3 shade_toon_d(LightingParams light, MaterialParams material) {
  return material.diffuseColor * light.color * toon_d(light, material.factors.z, material.factors.w);
}

device float minnaert(LightingParams params, float darkness) {
  float nl = max(dot(params.normal, params.direction), 0.0);
  float nv = max(dot(params.normal, params.view), 0.0);
  
  if(darkness <= 1.0)
    return nl*pow(max(nv*nl, 0.1), darkness - 1.0) * params.energy * params.attenuation;
  else
    return nl*pow(1.0001 - nv, darkness - 1.0) * params.energy * params.attenuation;
}
device float3 shade_minnaert(LightingParams light, MaterialParams material) {
  return material.diffuseColor * light.color * minnaert(light, material.factors.z);
}



// Specular Shaders
device float phong(LightingParams params, float hardness) {
  float3 halfAngle = normalize(params.view + params.direction);
  return pow(saturate(dot(params.normal, halfAngle)), hardness) * params.energy * params.attenuation;
}
device float3 shade_phong(LightingParams light, MaterialParams material) {
  return material.specularColor * light.color * phong(light, material.factors.x);
}

device float cooktorr(LightingParams params, float hardness) {
  float3 halfAngle = normalize(params.view + params.direction);
  float nh = max(dot(params.normal, halfAngle), 0.0);
  float nv = max(dot(params.normal, params.view ), 0.0);
  return pow(nh, hardness)/(0.1+nv) * params.energy * params.attenuation;
}
device float3 shade_cooktorr(LightingParams light, MaterialParams material) {
  return material.specularColor * light.color * cooktorr(light, material.factors.x);
}

device float toon_s(LightingParams params, float size, float smooth) {
  float3 halfAngle = normalize(params.view + params.direction);
  float angle = acos(dot(halfAngle, params.normal));
  if (angle < size)
    return params.energy * params.attenuation;
  if (angle >= (size + smooth) || smooth == 0.0)
    return 0.0;
  return (1.0 - ((angle - size)/smooth)) * params.energy * params.attenuation;
}
device float3 shade_toon_s(LightingParams light, MaterialParams material) {
  return material.specularColor * light.color * toon_s(light, material.factors.z, material.factors.w);
}

#define LIGHT_LOOP(DIFFUSE, SPECULAR, START, END) \
  for (int i = START; i < END; ++i) { \
    setLightToParams(&lightParams, input.location, lights[i]); \
    color += shade_##DIFFUSE(lightParams, materialParams); \
    color += shade_##SPECULAR(lightParams, materialParams); \
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

#define LIGHT_FUNC(DIFFUSE, SPECULAR, LIGHTS) \
fragment half4 f_##DIFFUSE##_##SPECULAR##_##LIGHTS(InputNormal input [[stage_in]], \
constant STR_Light *lights [[buffer(0)]], constant STR_Material *material [[buffer(1)]]) { \
  LightingParams lightParams; MaterialParams materialParams; \
  setupLightingParams(&lightParams, input.normal, input.view); \
  setupMaterialParams(&materialParams, *material); \
  float3 color = materialParams.ambiantColor; \
  LIGHT_LOOP(DIFFUSE, SPECULAR, 0, LIGHTS) \
  return half4(color.x, color.y, color.z, 1.0); \
}

#define LIGHT_FUNCS(DIFFUSE, SPECULAR) \
LIGHT_FUNC(DIFFUSE, SPECULAR, 1) \
LIGHT_FUNC(DIFFUSE, SPECULAR, 2) \
LIGHT_FUNC(DIFFUSE, SPECULAR, 4) \
LIGHT_FUNC(DIFFUSE, SPECULAR, 8)

LIGHT_FUNCS(lambert, phong)
LIGHT_FUNCS(lambert, cooktorr)
LIGHT_FUNCS(lambert, toon_s)

LIGHT_FUNCS(oren_nayer, phong)
LIGHT_FUNCS(oren_nayer, cooktorr)
LIGHT_FUNCS(oren_nayer, toon_s)

LIGHT_FUNCS(toon_d, phong)
LIGHT_FUNCS(toon_d, cooktorr)
LIGHT_FUNCS(toon_d, toon_s)

LIGHT_FUNCS(minnaert, phong)
LIGHT_FUNCS(minnaert, cooktorr)
LIGHT_FUNCS(minnaert, toon_s)




struct OutputColorNormal {
  float4 position [[ position    ]];
  float3 location [[ user(loc)   ]];
  float3 normal   [[ user(norm)  ]];
  float3 view     [[ user(view)  ]];
  float3 color    [[ user(color) ]];
};

struct InputColorNormal {
  float3 location [[ user(loc)   ]];
  float3 normal   [[ user(norm)  ]];
  float3 view     [[ user(view)  ]];
  float3 color    [[ user(color) ]];
};

vertex OutputColorNormal v_color_normal(const device packed_float3 *position [[ buffer(0) ]],
                                        const device packed_float3 *normal   [[ buffer(1) ]],
                                        const device packed_float3 *color0   [[ buffer(2) ]],
                                        constant     STR_Camera    *camera   [[ buffer(3) ]],
                                        constant     STR_Model     *model    [[ buffer(4) ]],
                                        unsigned     int            vid      [[ vertex_id ]]) {
  OutputColorNormal output;
  float4 location = model->model * float4(position[vid], 1.0);
  output.position = camera->projection * camera->view * location;
  output.location = location.xyz;
  output.normal = rotate_quat(model->rotation, float3(normal[vid]));
  output.view = camera->position.xyz - output.position.xyz;
  output.color = float3(color0[vid]);
  return output;
}

#define LIGHT_COLOR_FUNC(DIFFUSE, SPECULAR, LIGHTS) \
fragment half4 f_color_##DIFFUSE##_##SPECULAR##_##LIGHTS(InputColorNormal input [[stage_in]], \
constant STR_Light *lights [[buffer(0)]], constant STR_Material *material [[buffer(1)]]) { \
  LightingParams lightParams; MaterialParams materialParams; \
  setupLightingParams(&lightParams, input.normal, input.view); \
  setupMaterialParams(&materialParams, *material, input.color); \
  float3 color = materialParams.ambiantColor * input.color; \
  LIGHT_LOOP(DIFFUSE, SPECULAR, 0, LIGHTS) \
  return half4(color.x, color.y, color.z, 1.0); \
}

#define LIGHT_COLOR_FUNCS(DIFFUSE, SPECULAR) \
LIGHT_COLOR_FUNC(DIFFUSE, SPECULAR, 1) \
LIGHT_COLOR_FUNC(DIFFUSE, SPECULAR, 2) \
LIGHT_COLOR_FUNC(DIFFUSE, SPECULAR, 4) \
LIGHT_COLOR_FUNC(DIFFUSE, SPECULAR, 8)

LIGHT_COLOR_FUNCS(lambert, phong)
LIGHT_COLOR_FUNCS(lambert, cooktorr)
LIGHT_COLOR_FUNCS(lambert, toon_s)

LIGHT_COLOR_FUNCS(oren_nayer, phong)
LIGHT_COLOR_FUNCS(oren_nayer, cooktorr)
LIGHT_COLOR_FUNCS(oren_nayer, toon_s)

LIGHT_COLOR_FUNCS(toon_d, phong)
LIGHT_COLOR_FUNCS(toon_d, cooktorr)
LIGHT_COLOR_FUNCS(toon_d, toon_s)

LIGHT_COLOR_FUNCS(minnaert, phong)
LIGHT_COLOR_FUNCS(minnaert, cooktorr)
LIGHT_COLOR_FUNCS(minnaert, toon_s)



struct OutputTextureNormal {
  float4 position   [[ position   ]];
  float3 location   [[ user(loc)  ]];
  float3 normal     [[ user(norm) ]];
  float3 view       [[ user(view) ]];
  float2 coordinate [[ user(uv)   ]];
};

struct InputTextureNormal {
  float3 location   [[ user(loc)  ]];
  float3 normal     [[ user(norm) ]];
  float3 view       [[ user(view) ]];
  float2 coordinate [[ user(uv)   ]];
};

vertex OutputTextureNormal v_texture_normal(const device packed_float3 *position [[ buffer(0) ]],
                                            const device packed_float3 *normal   [[ buffer(1) ]],
                                            const device packed_float2 *uv0      [[ buffer(2) ]],
                                            constant     STR_Camera    *camera   [[ buffer(3) ]],
                                            constant     STR_Model     *model    [[ buffer(4) ]],
                                            unsigned     int            vid      [[ vertex_id ]]) {
  OutputTextureNormal output;
  float4 location = model->model * float4(position[vid], 1.0);
  output.position = camera->projection * camera->view * location;
  output.location = location.xyz;
  output.normal = rotate_quat(model->rotation, float3(normal[vid]));
  output.view = camera->position.xyz - output.position.xyz;
  output.coordinate = float2(uv0[vid]);
  return output;
}

#define LIGHT_TEXTURE_FUNC(DIFFUSE, SPECULAR, LIGHTS) \
fragment half4 f_texture_##DIFFUSE##_##SPECULAR##_##LIGHTS(InputTextureNormal input [[stage_in]], \
constant STR_Light *lights [[buffer(0)]], constant STR_Material *material [[buffer(1)]], \
texture2d<float> texture2D [[texture(0)]], sampler sampler2D [[sampler(0)]]) { \
  LightingParams lightParams; MaterialParams materialParams; \
  float4 texture = texture2D.sample(sampler2D, input.coordinate); \
  setupLightingParams(&lightParams, input.normal, input.view); \
  setupMaterialParams(&materialParams, *material, texture.xyz); \
  float3 color = materialParams.ambiantColor * texture.xyz; \
  LIGHT_LOOP(DIFFUSE, SPECULAR, 0, LIGHTS) \
  return half4(color.x, color.y, color.z, texture.w); \
}

#define LIGHT_TEXTURE_FUNCS(DIFFUSE, SPECULAR) \
LIGHT_TEXTURE_FUNC(DIFFUSE, SPECULAR, 1) \
LIGHT_TEXTURE_FUNC(DIFFUSE, SPECULAR, 2) \
LIGHT_TEXTURE_FUNC(DIFFUSE, SPECULAR, 4) \
LIGHT_TEXTURE_FUNC(DIFFUSE, SPECULAR, 8)

LIGHT_TEXTURE_FUNCS(lambert, phong)
LIGHT_TEXTURE_FUNCS(lambert, cooktorr)
LIGHT_TEXTURE_FUNCS(lambert, toon_s)

LIGHT_TEXTURE_FUNCS(oren_nayer, phong)
LIGHT_TEXTURE_FUNCS(oren_nayer, cooktorr)
LIGHT_TEXTURE_FUNCS(oren_nayer, toon_s)

LIGHT_TEXTURE_FUNCS(toon_d, phong)
LIGHT_TEXTURE_FUNCS(toon_d, cooktorr)
LIGHT_TEXTURE_FUNCS(toon_d, toon_s)

LIGHT_TEXTURE_FUNCS(minnaert, phong)
LIGHT_TEXTURE_FUNCS(minnaert, cooktorr)
LIGHT_TEXTURE_FUNCS(minnaert, toon_s)
