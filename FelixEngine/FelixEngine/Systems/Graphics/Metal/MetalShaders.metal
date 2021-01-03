//
//  MetalShaders.metal
//  FelixEngine
//
//  Created by Robert Crosby on 11/29/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#define NUM_LIGHTS 2
#define FOUR_LIGHTS 4

#include "MetalFunctions.h"



struct OutputCube {
  float4 position [[ position ]];
  float2 uv [[ user(uv) ]];
};

struct InputCube {
  float2 uv [[ user(uv) ]];
};

vertex OutputCube v_cube_gen(const device packed_float2 *vertices [[ buffer(0) ]],
                                          unsigned int   vid      [[ vertex_id ]]) {
  OutputCube output;
  output.position = float4(vertices[vid], 0.0, 1.0);
  output.uv = vertices[vid];
  return output;
}

fragment half4 f_cube_gen(InputCube input      [[ stage_in ]],
                    constant float4 &rotation  [[ buffer(0) ]],
                   texture2d<float> srcTexture [[ texture(0) ]],
                            sampler sampler2D  [[ sampler(0) ]]) {
  float3 loc = rotate_quat(rotation, float3(1, input.uv.x, input.uv.y));
  
  float theta = atan2(loc.y, loc.x);
  float r = sqrt(loc.x*loc.x + loc.y*loc.y);
  float phi = atan2(loc.z, r);
  
  float2 coords(0.5*(theta + M_PI_F) / M_PI_F, (M_PI_F/2 - phi) / M_PI_F);
  
  float4 color = srcTexture.sample(sampler2D, coords);
  return half4(color.r, color.g, color.b, color.a);
}


struct Vertex {
  float3 position [[ attribute(0) ]];
};

struct OutputShadeless {
  float4 position [[ position ]];
};

vertex OutputShadeless v_shadeless(Vertex  input  [[ stage_in    ]],
                      constant STR_Camera &camera [[ buffer(1)   ]],
                      constant STR_Model  *model  [[ buffer(2)   ]],
                      unsigned int         iid    [[ instance_id ]]) {
  OutputShadeless output;
  float4 location = model[iid].model * float4(input.position, 1.0);
  output.position = camera.projection * camera.view * location;
  return output;
}

fragment half4 f_shadeless(constant STR_Material *material [[ buffer(0) ]]) {
  float3 color = material->ambiant.xyz * material->ambiant.a;
  return half4(color.x, color.y, color.z, 1.0);
}



struct VertexColor {
  float3 position [[ attribute(0) ]];
  float3 color0   [[ attribute(1) ]];
};

struct OutputColor {
  float4 position [[ position    ]];
  float3 color    [[ user(color) ]];
};

struct InputColor {
  float3 color [[ user(color) ]];
};

vertex OutputColor v_color(VertexColor  input  [[ stage_in    ]],
                   constant STR_Camera &camera [[ buffer(2)   ]],
                   constant STR_Model  *model  [[ buffer(3)   ]],
                   unsigned int         iid    [[ instance_id ]]) {
  OutputColor output;
  float4 location = model[iid].model * float4(input.position, 1.0);
  output.position = camera.projection * camera.view * location;
  output.color = input.color0;
  return output;
}

fragment half4 f_color_shadeless(InputColor  input    [[ stage_in ]],
                      constant STR_Material *material [[ buffer(0) ]]) {
  float3 color = input.color * material->ambiant.xyz * material->ambiant.a;
  return half4(color.x, color.y, color.z, 1.0);
}



struct VertexTexture {
  float3 position [[ attribute(0) ]];
  float2 uv0      [[ attribute(1) ]];
};

struct OutputTexture {
  float4 position   [[ position ]];
  float2 coordinate [[ user(uv) ]];
};

struct InputTexture {
  float2 coordinate [[ user(uv) ]];
};

vertex OutputTexture v_texture(VertexTexture  input  [[ stage_in    ]],
                         constant STR_Camera &camera [[ buffer(2)   ]],
                         constant STR_Model  *model  [[ buffer(3)   ]],
                         unsigned int         iid    [[ instance_id ]]) {
  OutputTexture output;
  float4 location = model[iid].model * float4(input.position, 1.0);
  output.position = camera.projection * camera.view * location;
  output.coordinate = transform_coordinate(model[iid].texture, input.uv0);
  return output;
}

fragment half4 f_texture_shadeless(InputTexture  input     [[ stage_in   ]],
                          constant STR_Material *material  [[ buffer(0)  ]],
                               texture2d<float>  texture2D [[ texture(0) ]],
                                        sampler  sampler2D [[ sampler(0) ]]) {
  float4 texture = texture2D.sample(sampler2D, input.coordinate);
  return half4(texture * material->ambiant);
}



vertex OutputTexture video_vertex(VertexTexture  input     [[ stage_in  ]],
                              constant float4x4 &transform [[ buffer(2) ]],
                                   unsigned int  vid       [[ vertex_id ]]) {
  OutputTexture output;
  output.position = float4(input.position, 1.0);
  output.coordinate = transform_coordinate(transform, input.uv0);
  return output;
}

fragment float4 video_fragment_Y_CbCr(InputTexture input                    [[ stage_in   ]],
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
  float4 ycbcr = float4(capturedImageTextureY.sample(colorSampler, input.coordinate).r,
                        capturedImageTextureCbCr.sample(colorSampler, input.coordinate).rg, 1.0);
  
  // Return converted RGB color
  return ycbcrToRGBTransform * ycbcr;
}


struct VertexNormal {
  float3 position [[ attribute(0) ]];
  float3 normal   [[ attribute(1) ]];
};

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

vertex OutputNormal v_normal(VertexNormal  input  [[ stage_in    ]],
                      constant STR_Camera &camera [[ buffer(2)   ]],
                      constant STR_Model  *model  [[ buffer(3)   ]],
                      unsigned int         iid    [[ instance_id ]]) {
  OutputNormal output;
  float4 location = model[iid].model * float4(input.position, 1.0);
  output.position = camera.projection * camera.view * location;
  output.location = location.xyz;
  output.normal = rotate_quat(model[iid].rotation, float3(input.normal));
  output.view = camera.position.xyz - output.position.xyz;
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


struct VertexColorNormal {
  float3 position [[ attribute(0) ]];
  float3 normal   [[ attribute(1) ]];
  float3 color0   [[ attribute(2) ]];
};

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

vertex OutputColorNormal v_color_normal(VertexColorNormal  input  [[ stage_in    ]],
                                     constant STR_Camera  &camera [[ buffer(3)   ]],
                                     constant STR_Model   *model  [[ buffer(4)   ]],
                                     unsigned int          iid    [[ instance_id ]]) {
  OutputColorNormal output;
  float4 location = model[iid].model * float4(input.position, 1.0);
  output.position = camera.projection * camera.view * location;
  output.location = location.xyz;
  output.normal = rotate_quat(model[iid].rotation, input.normal);
  output.view = camera.position.xyz - output.position.xyz;
  output.color = input.color0;
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


struct VertexTextureNormal {
  float3 position [[ attribute(0) ]];
  float3 normal   [[ attribute(1) ]];
  float2 uv0      [[ attribute(2) ]];
};

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

vertex OutputTextureNormal v_texture_normal(VertexTextureNormal   input  [[ stage_in    ]],
                                            constant  STR_Camera &camera [[ buffer(3)   ]],
                                            constant  STR_Model  *model  [[ buffer(4)   ]],
                                            unsigned  int         iid    [[ instance_id ]]) {
  OutputTextureNormal output;
  float4 location = model[iid].model * float4(input.position, 1.0);
  output.position = camera.projection * camera.view * location;
  output.location = location.xyz;
  output.normal = rotate_quat(model[iid].rotation, input.normal);
  output.view = camera.position.xyz - output.position.xyz;
  output.coordinate = transform_coordinate(model->texture, input.uv0);
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



struct VertexTextureBasis {
  float3 position [[ attribute(0) ]];
  float3 normal   [[ attribute(1) ]];
  float4 tangent  [[ attribute(2) ]];
  float2 uv0      [[ attribute(3) ]];
};

struct OutputTextureBasis {
  float4   position   [[ position   ]];
  float3   location   [[ user(loc)  ]];
  float3   normal     [[ user(norm) ]];
  float3   bitangent  [[ user(btan) ]];
  float3   cotangent  [[ user(ctan) ]];
  float3   view       [[ user(view) ]];
  float2   coordinate [[ user(uv)   ]];
};

struct InputTextureBasis {
  float3   location   [[ user(loc)  ]];
  float3   normal     [[ user(norm) ]];
  float3   bitangent  [[ user(btan) ]];
  float3   cotangent  [[ user(ctan) ]];
  float3   view       [[ user(view) ]];
  float2   coordinate [[ user(uv)   ]];
};

vertex OutputTextureBasis v_texture_basis(VertexTextureBasis    input  [[ stage_in    ]],
                                          constant  STR_Camera &camera [[ buffer(4)   ]],
                                          constant  STR_Model  *model  [[ buffer(5)   ]],
                                          unsigned  int         iid    [[ instance_id ]]) {
  OutputTextureBasis output;
  float4 location = model[iid].model * float4(input.position, 1.0);
  output.position = camera.projection * camera.view * location;
  output.location = location.xyz;
  output.normal = rotate_quat(model[iid].rotation, input.normal);
  output.bitangent = rotate_quat(model[iid].rotation, input.tangent.xyz);
  output.cotangent = cross(output.normal, output.bitangent) * input.tangent.w;
  output.view = camera.position.xyz - output.position.xyz;
  output.coordinate = transform_coordinate(model->texture, input.uv0);
  return output;
}


fragment half4 f_pbr_shadeless(InputTextureBasis  input       [[stage_in]],
                               texture2d<float>   albedoTexture      [[texture(0)]],
                               texture2d<float>   specularTexture    [[texture(1)]],
                               texture2d<float>   roughnessTexture   [[texture(2)]],
                               texture2d<float>   metalnessTexture   [[texture(3)]],
                               texture2d<float>   emissiveTexture    [[texture(4)]],
                               texture2d<float>   occlusionTexture   [[texture(5)]],
                               texture2d<float>   normalsTexture     [[texture(6)]],
                               texturecube<float> environmentCubeMap [[texture(7)]]) {
  constexpr sampler defSampler(mip_filter::linear, mag_filter::linear, min_filter::linear);
  uint envLevels = environmentCubeMap.get_num_mip_levels();
  
  //return half4(albedoTexture.sample(defSampler, input.coordinate));
  //return half4(specularTexture.sample(defSampler, input.coordinate));
  //return half4(normalsTexture.sample(defSampler, input.coordinate));
  //return half4(roughnessTexture.sample(defSampler, input.coordinate));
  //return half4(metalnessTexture.sample(defSampler, input.coordinate));
  //return half4(emissiveTexture.sample(defSampler, input.coordinate));
  //return half4(occlusionTexture.sample(defSampler, input.coordinate));
  
  float3 albedo = albedoTexture.sample(defSampler, input.coordinate).rgb;
  float metalness = metalnessTexture.sample(defSampler, input.coordinate).r;
  float roughness = roughnessTexture.sample(defSampler, input.coordinate).r;
  
  // Outgoing light direction
  float3 Lo = normalize(input.view);
  
  // Determine normal
  float3x3 basis(input.bitangent, input.cotangent, input.normal);
  float3 N = normalize(basis * normalize(2.0 * normalsTexture.sample(defSampler, input.coordinate).xyz - 1.0));
  
  // Angle between surface normal and outgoing light direction.
  float cosLo = max(0.0, dot(N, Lo));
  
  // Specular reflection vector.
  float3 Lr = 2.0 * cosLo * N - Lo;
  
  // Fresnel reflectance at normal incidence
  float3 F0 = mix(float3(0.04), albedo, metalness);
  
  
  // Ambient lighting (IBL)
  
  // Sample diffuse irradiance at normal direction
  float3 irradiance = environmentCubeMap.sample(defSampler, N, level(0.8 * envLevels)).rgb;
  
  // Calculate Fresnel term for ambient lighting
  float3 F = fresnel_schlick(F0, cosLo);
  
  // Get diffuse contribution factor (as with direct lighting).
  float3 kd = mix(float3(1.0) - F, float3(0.0), metalness);
  
  float3 diffuseIBL = kd * albedo * irradiance;
  
  float3 specularIrradiance = environmentCubeMap.sample(defSampler, Lr, level(0.8 * roughness * envLevels)).rgb;
  
  // Split-sum approximation factors for Cook-Torrance specular BRDF.
  //float2 specularBRDF = specularBRDF_LUT.sample(defSampler, float2(cosLo, roughness));

  // Total specular IBL contribution.
  //float3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;
  float3 specularIBL = F0 * specularIrradiance;

      // Total ambient lighting contribution.
  float3 ambientLighting = diffuseIBL + specularIBL;
  
  
  return half4(ambientLighting.r, ambientLighting.g, ambientLighting.b, 1.0);
  
//
//  //float d = dot(normal, float3(0.0, 1.0, 0.0));
//
//  // Light Out Direction
//  float3 Lo = normalize(input.view);
//
  
  //float4 collect = diffuse.sample(defSampler, input.coordinate) * float4(d, d, d, 1.0);
  //collect += environment.sample(defSampler, normal);
  //return half4(collect);
}
